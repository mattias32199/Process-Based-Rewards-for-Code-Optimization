import string
import secrets
import os
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

def compute_performance_metrics(trajectories, save_path:str="") -> dict:
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
            save_solution(trajectory, save_path)
            speedup += 1

    perf_metrics = {
        'correct_format': correct_format / total * 100,
        'correct': correct / total * 100,
        'speedup': speedup / total * 100
    }

    return perf_metrics


def save_solution(trajectory, save_path) -> None:
    alphabet = string.ascii_letters + string.digits
    uuid = ''.join(secrets.choice(alphabet) for _ in range(8))   # 8-char
    imm_reward = trajectory['immediate_reward']
    speedup = imm_reward - 0.3
    reward_str = '_'.join(str(round(speedup, 4)).split('.'))
    task_id = trajectory['task_id']
    file_name= f"{task_id}-{reward_str}-{uuid}.txt"
    file_path = os.path.join(save_path, file_name)
    print(f'Saving... {file_name}')
    with open(file_path, 'w') as f:
        f.write(f"speedup: {speedup:.4f}\n")
        f.write("response:\n")
        f.write(trajectory['response'])

def write_metrics_csv(
    epoch: int, step: int,
    avg_reward: float, std_reward: float,
    per_format: float, per_correct: float, per_speedup: float,
    loss: float, kl: float, clip: float,
    csv_path:str
) -> None:
    with open(csv_path, 'a') as f:
        row = f"{epoch}, {step}, {avg_reward}, {std_reward}, {per_format}, {per_correct}, {per_speedup}, {loss}, {kl}, {clip}\n"
        f.write(row)
