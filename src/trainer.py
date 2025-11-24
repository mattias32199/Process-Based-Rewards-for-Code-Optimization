# multi-turn RL GSPO trainer
import numpy as np
import torch
from engine import UnifiedPolicyEngine

class MultiTurnRLTrainer():
    def __init__(self, config, verify_fn, profile_fn, reward_fn, construct_prompt_fn, parse_response_fn):
        """
        parameters:
            model: expects vllm model
        """
        # config yaml
        self.config = config

        # initialize model engine
        self.engine = UnifiedPolicyEngine(config.model.get("name"))

        self.max_turns = config.max_turns
        self.parallel_trajectories = config.parallel_trajectories

        # functions
        self.verify_fn = verify_fn # verification (correctness tests)
        self.profile_fn = profile_fn # profiling on hardware
        self.construct_prompt_fn = construct_prompt_fn # construct prompt from context manager
        self.parse_respose_fn = parse_response_fn
        self.reward_fn = reward_fn # calculate reward from verifyfication and profiling


    def train(self, dataloader):
        """
        Top-level training loop.
        Rollout -> Update Policy
        """
        for epoch in range(self.config.epochs):
            for batch in dataloader:
                # reinforcement learning rollout to generate trajectories
                trajectories = self.rollout(batch) # multi-turn interaction to generate trajectories
                loss, metrics = self.update_policy(trajectories)

    def rollout(self, batch):
        """
        batch -> tasks -> model -> trajectories
        """
        # initialize trajectories and context manager
        envs = [] # len(envs) = task x parallel_trajectories
        for task in batch:
            for _ in range(self.parallel_trajectories):
                envs.append(
                    {
                        "task_id": task["task_id"],
                        "context_buffer": {
                            "task_prompt": task["prompt"],
                            "solution_scalar": task["solution_scalar"],
                            "turns": [],
                        },
                        "trajectory": {
                            "task_id": task["task_id"],
                            "turns": [],
                        },
                    }
                )

        # multi-turn RL
        for turn in range(self.max_turns):
            # construct prompts
            prompts = [
                self.construct_prompt_fn(env["context_buffer"], turn) for env in envs
            ]
            # batched response generation
            gen_infos = self.engine.generate_batched_responses(prompts)

            for env, prompt, gen_info in zip(envs, prompts, gen_infos):
                response = gen_info["response_text"]
                input_ids_full = gen_info["input_ids_full"]
                token_log_probs_old = gen_info["token_log_probs_old"]
                prompt_len = gen_info["prompt_len"]
                response_len = gen_info["response_len"]

                cot, solution_simd = self.parse_response_fn(response)
                verification, profile = self.evaluate_response(response, turn)
                reward = self.reward_fn(verification, profile, turn)

                env["context_buffer"]["turns"].append(
                    {
                        "turn": turn,
                        "solution_simd": solution_simd,
                        "cot": cot,
                        "verification": verification,
                        "profile": profile,
                    }
                )

                env["trajectory"]["turns"].append(
                    {
                        "turn": turn,
                        "prompt": prompt,
                        "response": response,
                        "reward": float(reward),
                        "token_log_probs_old": token_log_probs_old,
                        "input_ids_full": input_ids_full,
                        "prompt_len": prompt_len,
                        "response_len": response_len,
                    }
                )

        trajectories = []
        for task_idx, task in enumerate(batch): # loop through each task
            # replace loop with ray later on?
            for sample in self.parallel_trajectories: # parallel samples/trajectories
                trajectory = self.generate_trajectory(task)
                trajectories.append(trajectory)

                # calculate metrics from trajectories?
        return trajectories

    def generate_trajectory(self, task):

        trajectory = {
            "task_id": task['task_id'],
            # "turns": [
            #     {
            #         "prompt": str,
            #         "response": str, # cot + code
            #         "summary": str,

            #         "reward": float,
            #         "return": float, # discounted return
            #         "log_probs_old": np.array
            #     }
            # ]
        }

        context_buffer = {
            "task_prompt": task['prompt'],  # Base task
            "solution_scalar": task["solution_scalar"]
            # "turns": [  # Minimal info for prompt construction
            #     {
            #         "turn": int,
            #         "code": str,
            #         "cot": str,  # cot or summarized cot
            #         "verification_feedback": dict,  # feedback
            #         "profiling_feedback": dict or None  # feedback
            #     }
            # ]
        }
        for turn in range(self.max_turns):
            prompt = self.construct_prompt_fn(context_buffer, turn)
            outputs = self.model.generate([prompt], self.config.sampling_params)
            outputs = outputs[0].output[0]
            log_prob_old = outputs.cumulative_logprob
            response = outputs.text
            cot, solution_simd = self.parse_respose_fn(response)
            verification, profile = self.evaluate_response(response, turn)
            reward = self.reward_fn(verification, profile, turn)

            context_buffer['turns'].append({
                'solution_simd': solution_simd,
                'cot': cot,
                'verification': verification,
                'profile': profile
            })

            trajectory["turns"].append({
                'prompt': prompt,
                'response': response,
                'reward': reward,
                'log_prob_old': log_prob_old
            })

        return trajectory

    def evaluate_response(self, response, turn):
        verification = self.verify_fn(response, turn)
        if verification['all']:
            profile = self.profile_fn(response, turn)
        else:
            profile = None
        return verification, profile

    def gspo_loss(self, old_log_probs, log_probs, ):
        """
        Needs:
            old_log_prob
            log_prob
            advantages
        """
