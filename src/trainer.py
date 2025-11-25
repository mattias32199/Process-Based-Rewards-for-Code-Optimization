import torch
from typing import List, Dict, Any, Tuple, Optional
from src.engine import UnifiedPolicyEngine
from src.config import TrainerConfig, ActorConfig
from src.rl_utils import compute_policy_loss_gspo
from src.rewards_utils import compute_advantages

class MultiTurnRLTrainer:
    def __init__(
        self,
        config: TrainerConfig,
        engine: UnifiedPolicyEngine, # Injected dependency
        verify_fn,
        profile_fn,
        reward_fn,
        construct_prompt_fn,
        parse_response_fn,
    ):
        self.config = config
        self.engine = engine # The Unified Engine
        self.verify_fn = verify_fn
        self.profile_fn = profile_fn
        self.reward_fn = reward_fn
        self.construct_prompt_fn = construct_prompt_fn
        self.parse_response_fn = parse_response_fn

    def train(self, dataloader):
        """Main Loop"""
        for epoch in range(self.config.epochs):
            for batch_idx, batch in enumerate(dataloader):
                
                # 1. Rollout (Inference Mode)
                # Returns trajectories with pre-calculated advantages (future credit assigned)
                trajectories = self.rollout(batch)
                
                # 2. Update (Training Mode)
                # on-policy
                loss1, metrics1 = self.update_policy(trajectories)
                # off-policy
                loss2, metrics2 = self.update_policy(trajectories)
                
                
                print(f"[Epoch {epoch} Step {batch_idx}] On-Policy"
                      f"Loss: {loss1:.4f} | "
                      f"KL: {metrics1['actor/ppo_kl']:.4f} | "
                      f"Clip: {metrics1['actor/pg_clipfrac']:.4f}")
                print(f"[Epoch {epoch} Step {batch_idx}] Off-Policy "
                      f"Loss: {loss2:.4f} | "
                      f"KL: {metrics2['actor/ppo_kl']:.4f} | "
                      f"Clip: {metrics2['actor/pg_clipfrac']:.4f}")

    def rollout(self, batch: List[Dict]) -> List[Dict]:
        """
        Generates data and performs credit assignment.
        """
        envs = self._init_envs(batch)

        for turn in range(self.config.max_turns):
            prompts = [self.construct_prompt_fn(e["context_buffer"], turn) for e in envs]

            # A. Generate Text (Engine handles inference mode)
            responses = self.engine.generate(
                prompts, 
                max_new_tokens=self.config.max_new_tokens,
                temperature=self.config.temperature
            )

            # B. Evaluate & Reward
            for i, (env, response) in enumerate(zip(envs, responses)):
                cot, sol_simd = self.parse_response_fn(response)
                verif, profile = self.evaluate_response(response, turn)
                
                # NOTE: This is the IMMEDIATE reward (e.g., 0.3 or speedup)
                reward = self.reward_fn(verif, profile, turn)
                
                # Update buffers
                env["context_buffer"]["turns"].append({
                    "turn": turn, "cot": cot, "solution": sol_simd
                })
                
                # Store trajectory data temporarily
                env["trajectory"]["turns"].append({
                    "turn": turn,
                    "prompt": prompts[i],
                    "response": response,
                    "reward": float(reward)
                })

        # C. Compute Old Logprobs (Batch Processing)
        flat_turns = []
        for env in envs:
            flat_turns.extend(env["trajectory"]["turns"])
            
        traj_prompts = [t["prompt"] for t in flat_turns]
        traj_resps = [t["response"] for t in flat_turns]
        
        # Switch Engine to Training/LogProb Mode (but no_grad for old policy)
        self.engine.set_train_mode()
        with torch.no_grad():
            old_log_probs, masks = self.engine.get_batch_logprobs(traj_prompts, traj_resps)
            
        # Distribute back to turn objects
        for i, t in enumerate(flat_turns):
            t["token_log_probs_old"] = old_log_probs[i].cpu()
            t["response_mask"] = masks[i].cpu()
        
        trajectories = [e["trajectory"] for e in envs]

        # D. Future Credit Assignment & Normalization
        # This injects "advantage" into every turn dict inside trajectories
        trajectories = compute_advantages(trajectories)
        
        print("\n[rollout] Trajectory rewards and advantages:")
        for traj_i, traj in enumerate(trajectories):
            print(f"Task ID: {traj['task_id']}")
            rewards = [t.get("reward") for t in traj["turns"]]
            advs = [t.get("advantage") for t in traj["turns"]]
            print(f"  traj {traj_i}:")
            print("    rewards:    ", rewards)
            print("    advantages: ", advs)
        
        return trajectories

    def update_policy(self, trajectories: List[Dict]):
        """
        Calculates loss and updates model.
        """
        # Flatten
        turns = [t for traj in trajectories for t in traj["turns"]]
        if not turns: return 0.0, {}

        # Prepare batch data
        prompts = [t["prompt"] for t in turns]
        responses = [t["response"] for t in turns]
        
        # Extract the ADVANTAGES calculated by compute_advantages
        # These already include gamma discounting and normalization
        advantages_list = [t["advantage"] for t in turns]
        advantages_tensor = torch.tensor(advantages_list, device=self.engine.device, dtype=torch.float32)

        # Retrieve pre-calculated old data
        old_log_probs_padded = torch.nn.utils.rnn.pad_sequence(
            [t["token_log_probs_old"].to(self.engine.device) for t in turns],
            batch_first=True
        )
        response_mask_padded = torch.nn.utils.rnn.pad_sequence(
            [t["response_mask"].to(self.engine.device) for t in turns],
            batch_first=True
        )
        
        # 1. Forward Pass (Gradient enabled)
        self.engine.set_train_mode()
        
        # Recalculate everything to ensure graph connectivity and correct padding
        current_log_probs, current_mask = self.engine.get_batch_logprobs(prompts, responses)
        
        # 2. Align Shapes
        min_len = min(current_log_probs.size(1), old_log_probs_padded.size(1))
        current_log_probs = current_log_probs[:, :min_len]
        old_log_probs_padded = old_log_probs_padded[:, :min_len]
        final_mask = current_mask[:, :min_len] * response_mask_padded[:, :min_len]

        # 3. Advantages (Broadcast scalar advantage to all tokens in the response)
        # advantages_tensor is (B,), we want (B, T)
        advantages = advantages_tensor.unsqueeze(1) * final_mask

        with torch.no_grad():
            print("\n[update_policy] current_log_probs shape:", tuple(current_log_probs.shape))
            print("[update_policy] old_log_probs_padded shape:", tuple(old_log_probs_padded.shape))
            print("[update_policy] current_mask nonzero per sample:",
                  current_mask[:, :min_len].sum(dim=1).tolist())
            print("[update_policy] response_mask_padded nonzero per sample:",
                  response_mask_padded[:, :min_len].sum(dim=1).tolist())
            print("[update_policy] final_mask nonzero per sample:",
                  final_mask.sum(dim=1).tolist())
            print("[update_policy] final_mask total nonzero:",
                  float(final_mask.sum().item()))
            print("[update_policy] advantages_tensor (per-turn):",
                  advantages_tensor.detach().cpu().tolist())
            print("[update_policy] advantages_tensor sum:",
                  float(advantages_tensor.detach().sum().item()))
            if advantages.numel() > 0:
                print("[update_policy] advantages (token-level) mean:",
                      float(advantages.detach().mean().item()))
                print("[update_policy] advantages (token-level) nonzero count:",
                      int((advantages.detach() != 0).sum().item()))
                
        # 4. Compute Loss (GSPO)
        pg_loss, metrics = compute_policy_loss_gspo(
            old_log_prob=old_log_probs_padded,
            log_prob=current_log_probs,
            advantages=advantages,
            response_mask=final_mask,
            config=self.config.actor
        )

        with torch.no_grad():
            print("[update_policy] pg_loss (tensor):", float(pg_loss.detach().item()))
            print("[update_policy] metrics:",
                  {k: float(v) for k, v in metrics.items()})

        # 5. Optimizer Step
        loss_val = self.engine.train_step(pg_loss)
        
        return loss_val, metrics

    def evaluate_response(self, response: str, turn: int):
        verification = self.verify_fn(response, turn)
        if verification.get("all", False):
            profile = self.profile_fn(response, turn)
        else:
            profile = None
        return verification, profile

    def _init_envs(self, batch):
        envs = []
        for task in batch:
            for _ in range(self.config.parallel_trajectories):
                envs.append({
                    "context_buffer": {"task": task, "turns": []},
                    "trajectory": {
                        "task_id": task["task_id"], # Ensure task_id exists for normalization
                        "turns": []
                    }
                })
        return envs