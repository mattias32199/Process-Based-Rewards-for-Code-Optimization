# src/metrics_util.py
# Metrics for detecting model collapse based on RAGEN paper
import torch
import numpy as np
from typing import List, Dict


def compute_gradient_norm(model_parameters) -> float:
    """
    Computes the L2 norm of the policy gradient.
    
    This is a stability check - spikes in gradient norm indicate:
    - Unstable parameter updates
    - Learning rate may be too high
    - Training data may be too noisy
    
    Args:
        model_parameters: Iterator of model parameters (e.g., model.parameters())
    
    Returns:
        float: L2 norm of gradients across all parameters
    """
    total_norm = 0.0
    param_count = 0
    
    for p in model_parameters:
        if p.grad is not None:
            param_norm = p.grad.data.norm(2)
            total_norm += param_norm.item() ** 2
            param_count += 1
    
    if param_count == 0:
        return 0.0
    
    total_norm = total_norm ** 0.5
    return total_norm


def compute_rollout_entropy(log_probs: torch.Tensor, mask: torch.Tensor) -> float:
    """
    Computes average token-level entropy across sampled responses.
    
    This is an exploration check - what to watch for:
    - Healthy: Stable decay trend over training
    - Collapse: Rapid drop (model becomes deterministic/repetitive - "Echo Trap")
    - Instability: Erratic changes correlate with collapsed reasoning
    
    Formula: H(π) = -Σ p(x) log p(x)
    
    Args:
        log_probs: (B, T, V) - Log probabilities from model output (before gathering)
                   OR (B, T) - Token log probabilities (after gathering)
        mask: (B, T) - Mask for valid tokens (1.0 for valid, 0.0 for padding)
    
    Returns:
        float: Average entropy across all valid tokens
    """
    with torch.no_grad():
        # If log_probs is 2D, we need to convert to entropy differently
        # For token-level log probs (already gathered), we can't directly compute entropy
        # We need the full distribution log_probs (B, T, V)
        
        if log_probs.dim() == 3:
            # Full distribution case: (B, T, V)
            # Convert log probs to probs
            probs = torch.exp(log_probs)
            # Entropy: -Σ p(x) log p(x)
            entropy = -(probs * log_probs).sum(dim=-1)  # (B, T)
        elif log_probs.dim() == 2:
            # Already gathered token log probs: (B, T)
            # We can't compute full entropy from this, but we can estimate
            # uncertainty from the magnitude of log probs
            # Higher magnitude (more negative) = more certain
            # We'll return negative log prob as a proxy for uncertainty
            entropy = -log_probs  # (B, T)
        else:
            raise ValueError(f"Expected log_probs to be 2D or 3D, got {log_probs.dim()}D")
        
        # Apply mask and compute mean
        masked_entropy = entropy * mask
        total_entropy = masked_entropy.sum()
        total_tokens = mask.sum()
        
        if total_tokens == 0:
            return 0.0
        
        avg_entropy = (total_entropy / total_tokens).item()
        return avg_entropy


def compute_reward_variance(trajectories: List[Dict], group_by: str = 'task_id') -> Dict[str, float]:
    """
    Computes in-group reward variance (standard deviation).
    
    This is an early warning signal:
    - High variance: Model is exploring diverse strategies (healthy)
    - Sudden drop: Model stopped exploring (happens BEFORE reward collapse)
    
    Groups rollouts by prompt/task and calculates variance within each group.
    
    Args:
        trajectories: List of trajectory dicts containing 'reward' and grouping key
        group_by: Key to group trajectories by (default: 'task_id')
    
    Returns:
        dict: Contains 'overall_std', 'per_group_std', and 'group_count'
    """
    if len(trajectories) == 0:
        return {'overall_std': 0.0, 'per_group_std': {}, 'group_count': 0}
    
    # Group trajectories by task
    groups = {}
    for traj in trajectories:
        group_id = traj.get(group_by, 'unknown')
        if group_id not in groups:
            groups[group_id] = []
        groups[group_id].append(traj['reward'])
    
    # Compute variance per group
    per_group_std = {}
    all_rewards = []
    
    for group_id, rewards in groups.items():
        rewards_array = np.array(rewards)
        all_rewards.extend(rewards)
        
        if len(rewards) > 1:
            std = float(np.std(rewards_array))
        else:
            std = 0.0
        
        per_group_std[group_id] = std
    
    # Compute overall standard deviation
    if len(all_rewards) > 1:
        overall_std = float(np.std(all_rewards))
    else:
        overall_std = 0.0
    
    return {
        'overall_std': overall_std,
        'per_group_std': per_group_std,
        'group_count': len(groups)
    }


def compute_average_reward(trajectories: List[Dict]) -> Dict[str, float]:
    """
    Computes average reward/success rate across rollouts.
    
    This is the standard performance metric:
    - Plateau/drop signals degraded task performance
    
    Args:
        trajectories: List of trajectory dicts containing 'reward'
    
    Returns:
        dict: Contains 'mean', 'std', 'min', 'max', 'count'
    """
    if len(trajectories) == 0:
        return {
            'mean': 0.0,
            'std': 0.0,
            'min': 0.0,
            'max': 0.0,
            'count': 0
        }
    
    rewards = [traj['reward'] for traj in trajectories]
    rewards_array = np.array(rewards)
    
    return {
        'mean': float(np.mean(rewards_array)),
        'std': float(np.std(rewards_array)),
        'min': float(np.min(rewards_array)),
        'max': float(np.max(rewards_array)),
        'count': len(rewards)
    }


def compute_all_metrics(
    model_parameters,
    log_probs: torch.Tensor,
    mask: torch.Tensor,
    trajectories: List[Dict],
    grad_norm: float = None
) -> Dict[str, any]:
    """
    Convenience function to compute all model collapse metrics at once.
    
    Args:
        model_parameters: Model parameters for gradient norm computation
        log_probs: Log probabilities for entropy computation
        mask: Token mask for entropy computation
        trajectories: Trajectories for reward statistics
        grad_norm: Pre-computed gradient norm (optional, will compute if None)
    
    Returns:
        dict: All metrics organized by category
    """
    metrics = {}
    
    # 1. Gradient Norm (Stability Check)
    if grad_norm is not None:
        metrics['gradient_norm'] = grad_norm
    else:
        metrics['gradient_norm'] = compute_gradient_norm(model_parameters)
    
    # 2. Rollout Entropy (Exploration Check)
    metrics['rollout_entropy'] = compute_rollout_entropy(log_probs, mask)
    
    # 3. In-Group Reward Variance (Early Warning)
    variance_metrics = compute_reward_variance(trajectories)
    metrics['reward_variance'] = variance_metrics['overall_std']
    metrics['reward_variance_per_group'] = variance_metrics['per_group_std']
    metrics['num_groups'] = variance_metrics['group_count']
    
    # 4. Average Reward (Performance Metric)
    reward_metrics = compute_average_reward(trajectories)
    metrics['avg_reward'] = reward_metrics['mean']
    metrics['reward_std'] = reward_metrics['std']
    metrics['reward_min'] = reward_metrics['min']
    metrics['reward_max'] = reward_metrics['max']
    
    return metrics


def print_collapse_metrics(metrics: Dict, epoch: int, step: int, prefix: str = ""):
    """
    Prints model collapse metrics in a formatted way.
    
    Args:
        metrics: Dictionary of metrics from compute_all_metrics
        epoch: Current epoch number
        step: Current step/batch number
        prefix: Optional prefix for the log line (e.g., "On-Policy" or "Off-Policy")
    """
    print(f"\n{'='*80}")
    print(f"[Epoch {epoch} Step {step}] {prefix} Model Collapse Metrics")
    print(f"{'='*80}")
    
    # Stability Check
    print(f"🔧 STABILITY:     Gradient Norm = {metrics.get('gradient_norm', 0.0):.6f}")
    print(f"                  {'⚠️  SPIKE DETECTED!' if metrics.get('gradient_norm', 0.0) > 10.0 else '✓ Stable'}")
    
    # Exploration Check
    entropy = metrics.get('rollout_entropy', 0.0)
    print(f"\n🎲 EXPLORATION:   Rollout Entropy = {entropy:.6f}")
    print(f"                  {'⚠️  Very Low - Possible Echo Trap!' if entropy < 0.1 else '✓ Exploring'}")
    
    # Early Warning Signal
    variance = metrics.get('reward_variance', 0.0)
    print(f"\n📊 DIVERSITY:     Reward Std Dev = {variance:.6f}")
    print(f"                  Groups: {metrics.get('num_groups', 0)}")
    print(f"                  {'⚠️  Low Variance - Model may be collapsing!' if variance < 0.01 else '✓ Diverse responses'}")
    
    # Performance
    avg_reward = metrics.get('avg_reward', 0.0)
    reward_std = metrics.get('reward_std', 0.0)
    print(f"\n🎯 PERFORMANCE:   Avg Reward = {avg_reward:.6f} ± {reward_std:.6f}")
    print(f"                  Range: [{metrics.get('reward_min', 0.0):.4f}, {metrics.get('reward_max', 0.0):.4f}]")
    
    print(f"{'='*80}\n")
