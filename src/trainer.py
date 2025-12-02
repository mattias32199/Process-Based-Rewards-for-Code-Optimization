# src/trainer.py
# multi-turn RL GSPO trainer
import torch
from src.engine import UnifiedPolicyEngine
from src.verifier import verify
from src.config import TrainerConfig
from src.reward_util import compute_immediate_reward, compute_advantages
from src.rl_util import compute_policy_loss_gspo
from src.train_util import construct_user_prompt, get_system_prompt, parse_response

class MultiTurnRLTrainer():
    def __init__(self,
        config: TrainerConfig,
    ):
        """
        Class for MultiTurnRL Trainer
        """
        # config yaml
        self.config = config

        # initialize training/inference engine
        self.config.engine.debug = self.config.debug
        self.engine = UnifiedPolicyEngine(config.engine)
        #

        # retrieve training config
        self.max_turns = config.max_turns
        self.parallel_trajectories = config.parallel_trajectories
        self.use_cot = config.use_cot
        self.debug = config.debug
        self.mini_batch_size = config.mini_batch_size


    def train(self, dataloader):
        """
        Top-level training loop.
        Rollout -> Update Policy
        """
        for epoch in range(self.config.epochs): # loop through epochs
            for batch_idx, batch in enumerate(dataloader): # loop through batches
                # 1. per-batch rollout
                trajectories = self.rollout(batch['tasks'])

                # 2. calculate policy gradients to update policy
                # on-policy update
                on_policy_loss, on_policy_metrics = self.update_policy(trajectories)
                # off-policy update
                off_policy_loss, off_policy_metrics = self.update_policy(trajectories)

                # print metrics
                print(f"[Epoch {epoch} Step {batch_idx}] On-Policy"
                        f"Loss: {on_policy_loss:.4f} | "
                        f"KL: {on_policy_metrics['actor/ppo_kl']:.4f} | "
                        f"Clip: {on_policy_metrics['actor/pg_clipfrac']:.4f}")
                print(f"[Epoch {epoch} Step {batch_idx}] Off-Policy "
                        f"Loss: {off_policy_loss:.4f} | "
                        f"KL: {off_policy_metrics['actor/ppo_kl']:.4f} | "
                        f"Clip: {off_policy_metrics['actor/pg_clipfrac']:.4f}")

                del trajectories


    def rollout(self, batch: list[dict]) -> list[dict]:
        """
        batch -> tasks -> model -> trajectories
        """
        # initialize trajectories and context managers
        envs = self.init_envs(batch)

        # multi-turn RL
        for turn in range(self.max_turns):
            # A. construct prompts
            prompts = self.construct_prompts(envs, turn)

            # B. batched response generation
            responses = self.engine.generate_responses(
                prompts,
                max_new_tokens=self.config.max_new_tokens,
                temperature=self.config.temperature
            )

            # C. Evaluation and calculating immediate rewards
            envs = self.evaluate(envs, prompts, responses, turn)

        # D. Future credit assignment & Advantage calculation
        # flatten parallel trajectories
        trajectories = [env["trajectory"] for env in envs]
        trajectories = compute_advantages(trajectories)

        # E. Flatten trajectories (all the way)
        trajectories = [turn for trajectory in trajectories for turn in trajectory['turns']]

        # F. Compute "old" (on-policy) logprobs
        # extract prompts and responses
        old_log_probs, masks = self.get_log_probs(
            trajectories, requires_grad=False, mini_batch_size=self.mini_batch_size
        )
        # repack logprobs and masks
        for i, t in enumerate(trajectories):
            t["token_log_probs_old"] = old_log_probs[i].detach().cpu()
            t["response_mask"] = masks[i].detach().cpu()

        return trajectories


    def update_policy(self, trajectories: list[dict]):
        # A. Calculate current/new log probs
        current_log_probs, current_mask = self.get_log_probs(
            trajectories, requires_grad=True, mini_batch_size=self.mini_batch_size
        )

        # B. Process trajectories for policy loss calculation
        gspo_variables = self.process_trajectories(trajectories, current_log_probs, current_mask)

        # C. Compute policy loss using GSPO
        pg_loss, metrics = compute_policy_loss_gspo(
            old_log_prob=gspo_variables['old_log_probs_padded'],
            log_prob=gspo_variables['current_log_probs'],
            advantages=gspo_variables['advantages'],
            response_mask=gspo_variables['final_mask'],
            config=self.config.gspo
        )

        # D. Update training engine
        loss_val = self.engine.update_training_engine(pg_loss)

        return loss_val, metrics

    def construct_prompts(self, envs, turn):
        """
        Construct prompts and apply tokenizer chat template.
        """
        prompts = []
        system_prompt = get_system_prompt(self.use_cot)
        for env in envs:
            user_prompt = construct_user_prompt(env["context_buffer"], turn, self.use_cot)
            messages = [
                {"role": "system", "content": system_prompt},
                {"role": "user",   "content": user_prompt}
            ]
            p = self.engine.tokenizer.apply_chat_template(
                messages,
                tokenize=False,
                add_generation_prompt=True
            )
            prompts.append(p)
        return prompts


    def evaluate(self, envs, prompts, responses, turn):
        for i, (env, response) in enumerate(zip(envs, responses)):
            """
            eval should contain 'outcome' and 'feedback'
            """
            entrypoint_simd = env['context_buffer']['task']['entrypoint_simd']
            # parse model response
            eval = parse_response(response, entrypoint_simd, self.use_cot)
            sol_simd = eval['simd_solution']
            cot = eval['cot']

            # only perform verification if format is correct
            if eval['correct_format'] and eval['parse_solution']:
                eval = verify(
                    env['context_buffer']['task'],
                    sol_simd
                )
            eval['task_id'] = entrypoint_simd = env['context_buffer']['task']['task_id']
            # immediate reward
            reward = compute_immediate_reward(eval)

            # Update buffers
            env["context_buffer"]["turns"].append({
                "turn": turn, "cot": cot, "solution": sol_simd,
                "eval": eval
            })

            # Store trajectory data temporarily
            env["trajectory"]["turns"].append({
                "turn": turn,
                "prompt": prompts[i],
                "response": response,
                "reward": float(reward)
            })
        return envs

    def init_envs(self, batch: list[dict]) -> list:
        """
        Util function to initialize trajectories and context buffers.
        """
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

    def get_log_probs(self, trajectories, requires_grad, mini_batch_size=8):
        """
        Batched wrapper for engine.generate_log_probs to prevent OOM.
        Direct replacement for the original method.
        """
        prompts = [t["prompt"] for t in trajectories]
        responses = [t["response"] for t in trajectories]

        # Guard against empty updates
        if not prompts:
            return torch.tensor([]).to(self.engine.device), torch.tensor([]).to(self.engine.device)

        self.engine.set_train_mode()

        # 1. Process in Mini-Batches
        batches_lp, batches_mask = [], []

        for i in range(0, len(prompts), mini_batch_size):
            p_chunk = prompts[i : i + mini_batch_size]
            r_chunk = responses[i : i + mini_batch_size]

            # Control gradients based on phase
            with torch.set_grad_enabled(requires_grad):
                lp, mask = self.engine.generate_log_probs(p_chunk, r_chunk)

            # If Rollout (no grad), offload to CPU to save VRAM immediately
            if not requires_grad:
                lp, mask = lp.cpu(), mask.cpu()
                # aggressive cleanup
                del p_chunk, r_chunk
                torch.cuda.empty_cache()

            batches_lp.append(lp)
            batches_mask.append(mask)

        # 2. Dynamic Re-assembly
        # Find global max length among all batches to align dimensions
        max_len = max(b.size(1) for b in batches_lp)

        # Helper to pad (0, pad_right) with value 0
        def pad_tensor(t, target_len):
            return torch.nn.functional.pad(t, (0, target_len - t.size(1)), value=0)

        # Concatenate all batches
        full_lp = torch.cat([pad_tensor(b, max_len) for b in batches_lp], dim=0)
        full_mask = torch.cat([pad_tensor(b, max_len) for b in batches_mask], dim=0)

        return full_lp, full_mask

    def process_trajectories(self, trajectories, current_log_probs, current_mask):
        # Extract advantages
        advantages_list = [t["advantage"] for t in trajectories]
        advantages_tensor = torch.tensor(advantages_list, device=self.engine.device, dtype=torch.float32)

        # Retrieve pre-calculated old data
        old_log_probs_padded = torch.nn.utils.rnn.pad_sequence(
            [t["token_log_probs_old"].to(self.engine.device) for t in trajectories],
            batch_first=True
        )
        response_mask_padded = torch.nn.utils.rnn.pad_sequence(
            [t["response_mask"].to(self.engine.device) for t in trajectories],
            batch_first=True
        )

        min_len = min(current_log_probs.size(1), old_log_probs_padded.size(1))
        current_log_probs = current_log_probs[:, :min_len]
        old_log_probs_padded = old_log_probs_padded[:, :min_len]
        final_mask = current_mask[:, :min_len] * response_mask_padded[:, :min_len]

        advantages = advantages_tensor.unsqueeze(1) * final_mask

        return {
            'current_log_probs': current_log_probs,
            'old_log_probs_padded': old_log_probs_padded,
            'advantages': advantages,
            'final_mask': final_mask
        }
