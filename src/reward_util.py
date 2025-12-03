# src/reward_util.py
import numpy as np

def compute_immediate_reward(eval: dict) -> float:
    """
    Computes immediate reward
    0.3*correctness + 1*speedup
    """
    if not eval['correct_format']:
        reward = -0.1
    else:
        reward = 0.0
        if eval['correct']:
            try:
                reward += 0.3
                if not eval['avg_speedup']:
                    eval['avg_speedup'] = 0.0
                reward += eval['avg_speedup']
            except Exception as e:
                print('PINEAPPLE')
                print(eval)
                print(e)
                raise NotImplementedError
    return reward


def batch_immediate_rewards(
    trajectories: list[dict],
    num_turns: int = 5
) -> np.ndarray:
    """
    Unpacks trajectories and packs rewards into array to
    batch future credit assignment.
    """
    # unpack trajectories and pack rewards
    rewards = np.array([
        [turn["reward"] for turn in traj["turns"]]
        for traj in trajectories
    ], dtype=np.float32)
    return rewards

def compute_future_credit(
    rewards: np.ndarray, # (batch_size, num_turns)
    gamma: float = 0.4 # kevin found 0.4 w/ sum aggregation to be most effective
) -> np.ndarray:
    """
    Assigns future credit using sum aggregation
    (rewards -> return)
    """
    batch_size, num_turns = rewards.shape
    future_credit = np.zeros_like(rewards)
    future_credit[:, -1] = rewards[:, -1]
    for t in range(num_turns - 2, -1, -1):
        # see Kevin 4.3
        future_credit[:, t] = rewards[:, t] + gamma * future_credit[:, t + 1]
    return future_credit


def get_task_indices(trajectories):
    task_indices = {}
    for i, traj in enumerate(trajectories):
        task_id = traj['task_id']
        if task_id not in task_indices:
            task_indices[task_id] = []
        task_indices[task_id].append(i)
    return task_indices

def normalize_rewards_per_task(batched_rewards, task_indices, eps=1e-8) -> np.ndarray:
    advantages = np.zeros_like(batched_rewards)
    for task_id, indices in task_indices.items():
        task_rewards = batched_rewards[indices]  # (num_traj_for_task, num_turns)
        mean = task_rewards.mean()
        std = task_rewards.std()
        advantages[indices] = (task_rewards - mean) / (std + eps)
        print(f"[normalize_rewards_per_task] task_id: {task_id} | "
              f"mean: {mean:.6f} | std: {std:.6f}")
    return advantages

def compute_advantages(
    trajectories: list[dict]
):
    """
    Runs full pipeline:
        (immediate rewards -> future credit assignment -> advantages)
    """
    batched_rewards = batch_immediate_rewards(trajectories) # batched immediate rewards
    batched_rewards = compute_future_credit(batched_rewards) # batched rewards w/ future credit assignment

    task_indices = get_task_indices(trajectories) # group indices by task
    advantages = normalize_rewards_per_task(batched_rewards, task_indices) # batched advantage calculation

    for i, traj in enumerate(trajectories): # repack advantages into trajectories
        for t, turn in enumerate(traj["turns"]):
            turn["advantage"] = float(advantages[i, t])

    return trajectories
