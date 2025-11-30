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
        self.engine = UnifiedPolicyEngine(config.engine)
        #

        # retrieve training config
        self.max_turns = config.max_turns
        self.parallel_trajectories = config.parallel_trajectories
        self.use_cot = config.use_cot
        self.debug = config.debug
        self.vram_verbose = getattr(config, "vram_verbose", False)


    def train(self, dataloader):
        """
        Top-level training loop.
        Rollout -> Update Policy
        """
        for epoch in range(self.config.epochs): # loop through epochs
            for batch_idx, batch in enumerate(dataloader): # loop through batches
                # 1. per-batch rollout
                if self.debug:
                    print('batch size (num tasks in batch): ', len(batch['tasks']))
                
                self.log_vram(f"epoch={epoch} batch={batch_idx} BEFORE rollout")
                trajectories = self.rollout(batch['tasks'])
                self.log_vram(f"epoch={epoch} batch={batch_idx} AFTER rollout")

                # 2. calculate policy gradients to update policy
                # on-policy update
                on_policy_loss, on_policy_metrics = self.update_policy(trajectories)
                self.log_vram(f"epoch={epoch} batch={batch_idx} AFTER on-policy update")
                # off-policy update
                off_policy_loss, off_policy_metrics = self.update_policy(trajectories)
                self.log_vram(f"epoch={epoch} batch={batch_idx} AFTER off-policy update")


                # print metrics
                if self.debug:
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

            if self.debug:
                print(f"turn {turn}")
                print("***********************")
                print(len(prompts),len(prompts[0]))
                print("***********************")

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
        old_log_probs, masks = self.get_log_probs(trajectories, requires_grad=False)
        # repack logprobs and masks
        for i, t in enumerate(trajectories):
            t["token_log_probs_old"] = old_log_probs[i].detach().cpu()
            t["response_mask"] = masks[i].detach().cpu()

        return trajectories


    def update_policy(self, trajectories: list[dict]):
        # A. Calculate current/new log probs
        current_log_probs, current_mask = self.get_log_probs(trajectories, requires_grad=True)

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
            task_id = env['context_buffer']['task']['task_id']
            eval = parse_response(response, entrypoint_simd, task_id , self.use_cot)
            sol_simd = eval['simd_solution']
            cot = eval['cot']

            # only perform verification if format is correct
            if eval['correct_format'] and eval['parse_solution']:
                eval = verify(
                    env['context_buffer']['task'],
                    sol_simd
                )

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

    def get_log_probs(self, trajectories, requires_grad):
        """
        current/new logprobs require grad
        """
        # unpack prompts and responses
        prompts = [t["prompt"] for t in trajectories]
        responses = [t["response"] for t in trajectories]
        self.engine.set_train_mode()
        if requires_grad:
            old_log_probs, masks = self.engine.generate_log_probs(prompts, responses)
        else:
            with torch.no_grad():
                old_log_probs, masks = self.engine.generate_log_probs(prompts, responses)
        return old_log_probs, masks

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
    
    # --- Helper ----

    def log_vram(self, tag: str):
        if not (self.vram_verbose and torch.cuda.is_available()):
            return

        device = self.engine.device if hasattr(self.engine, "device") else torch.device("cuda")
        allocated = torch.cuda.memory_allocated(device) / (1024 ** 3)
        reserved  = torch.cuda.memory_reserved(device) / (1024 ** 3)
        max_alloc = torch.cuda.max_memory_allocated(device) / (1024 ** 3)

        print(
            f"[VRAM][{tag}] "
            f"allocated={allocated:.2f} GB, "
            f"reserved={reserved:.2f} GB, "
            f"max_alloc={max_alloc:.2f} GB"
        )
