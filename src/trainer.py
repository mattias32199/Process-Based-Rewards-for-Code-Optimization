# multi-turn RL GSPO trainer
import numpy as np

class MultiTurnRLTrainer():
    def __init__(self, model, config, verify_fn, profile_fn, reward_fn, construct_prompt_fn, parse_response_fn):
        """
        parameters:
            model: expects vllm model
        """
        # tokenizer?
        # config yaml
        self.config = config
        self.model = model
        self.max_turns = config.max_turns
        self.parallel_trajectories = config.parallel_trajectories

        # functions
        self.verify_fn = verify_fn # verification (correctness tests)
        self.profile_fn = profile_fn # profiling on hardware
        self.construct_prompt_fn = construct_prompt_fn # construct prompt from context manager
        self.parse_respose_fn = parse_response_fn
        self.reward_fn = reward_fn # calculate reward from verifyfication and profiling


    def train(self, dataloader):
        for epoch in range(self.config.epochs):
            for batch in dataloader:
                trajectories = self.rollout(batch) # multi-turn interaction to generate trajectories

        raise NotImplementedError

    def rollout(self, batch):
        """
        batch -> tasks -> model -> trajectories
        """
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
