import torch
import numpy as np
from src.reward_util import get_task_indices

def compute_gradient_norm(model_parameters) -> float:
    """
    Computes the L2 norm of the policy gradient.
    Spikers in gradiet norm indicate model collapse and instability.
    model_parameters: model.parameters()
    """
    total_norm = torch.nn.utils.clip_grad_norm_(model_parameters, max_norm=float('inf'))
    return total_norm.item()

def compute_reward_distribution(trajectories) -> tuple[float, float]:
    """
    std is calculated per task
    then average of all stds is calculated
    """
    fca_rewards = np.array([traj['fca_reward'] for traj in trajectories])

    task_stds = []

    task_indices = get_task_indices(trajectories)
    for task_id, indices in task_indices.items():
        task_rewards = fca_rewards[indices]  # (num_traj_for_task, num_turns)
        std = task_rewards.std()
        task_stds.append(std)

    average_reward_std = float(np.mean(task_stds))
    average_reward = float(np.mean(fca_rewards))
    return average_reward, average_reward_std

def compute_performance_metrics(trajectories) -> dict:
    total = len(trajectories)
    correct_format = 0
    correct = 0
    speedup = 0
    for trajectory in trajectories:
        immediate_reward = trajectory['immediate_reward']
        if immediate_reward > -0.1:
            correct_format += 1
        if immediate_reward > 0.3:
            correct += 1
        if immediate_reward > 1.5:
            speedup += 1

    perf_metrics = {
        'correct_format': correct_format / total * 100,
        'correct': correct / total * 100,
        'speedup': speedup / total * 100
    }

    return perf_metrics
