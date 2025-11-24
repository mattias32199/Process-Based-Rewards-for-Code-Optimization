# rl_utils.py
import torch
from typing import Tuple, Dict, Any, Optional
from src.config import ActorConfig 

class VerlF:
    @staticmethod
    def masked_mean(x: torch.Tensor, mask: torch.Tensor) -> torch.Tensor:
        if mask.dtype != x.dtype:
            mask_f = mask.to(dtype=x.dtype)
        else:
            mask_f = mask
        denom = mask_f.sum()
        denom = torch.clamp(denom, min=1.0)
        return (x * mask_f).sum() / denom


verl_F = VerlF()


def agg_loss(loss_mat, loss_mask, loss_agg_mode):
    if loss_agg_mode == "seq-mean-token-mean":
        seq_mask = torch.sum(loss_mask, dim=-1)
        seq_losses = torch.sum(loss_mat * loss_mask, dim=-1) / (seq_mask + 1e-8)
        seq_mask = (seq_mask > 0).float()
        loss = verl_F.masked_mean(seq_losses, seq_mask)
        return loss
    return 0.0


def compute_policy_loss_gspo(
    old_log_prob: torch.Tensor,
    log_prob: torch.Tensor,
    advantages: torch.Tensor,
    response_mask: torch.Tensor,
    loss_agg_mode: str = "seq-mean-token-mean",
    config: Optional[ActorConfig] = None,
    rollout_is_weights: torch.Tensor | None = None,
) -> Tuple[torch.Tensor, Dict[str, Any]]:
    assert config is not None

    clip_ratio_low = config.clip_ratio_low if config.clip_ratio_low is not None else config.clip_ratio
    clip_ratio_high = config.clip_ratio_high if config.clip_ratio_high is not None else config.clip_ratio

    # token-level log ratio
    negative_approx_kl = log_prob - old_log_prob  # (B, T)

    # sequence-average KL for GSPO
    seq_lengths = response_mask.sum(dim=-1).clamp(min=1.0)
    negative_approx_kl_seq = (negative_approx_kl * response_mask).sum(dim=-1) / seq_lengths

    # Importance Sampling Ratio with Straight-Through Estimator logic
    # log s_i,t = sg[log s_i] + log_prob - sg[log_prob]
    log_seq_importance_ratio = (
        log_prob
        - log_prob.detach()
        + negative_approx_kl_seq.detach().unsqueeze(-1)
    )
    # Clamp for numerical stability
    log_seq_importance_ratio = torch.clamp(log_seq_importance_ratio, max=10.0)
    seq_importance_ratio = torch.exp(log_seq_importance_ratio)

    pg_losses1 = -advantages * seq_importance_ratio
    pg_losses2 = -advantages * torch.clamp(
        seq_importance_ratio,
        1.0 - clip_ratio_low,
        1.0 + clip_ratio_high,
    )
    pg_losses = torch.maximum(pg_losses1, pg_losses2)

    if rollout_is_weights is not None:
        pg_losses = pg_losses * rollout_is_weights

    pg_loss = agg_loss(
        loss_mat=pg_losses,
        loss_mask=response_mask,
        loss_agg_mode=loss_agg_mode,
    )

    # Metrics
    with torch.no_grad():
        pg_clipfrac = verl_F.masked_mean(
            (pg_losses2 > pg_losses1).float(),
            response_mask,
        )
        ppo_kl = verl_F.masked_mean(-negative_approx_kl, response_mask)

    pg_metrics = {
        "actor/pg_clipfrac": pg_clipfrac.detach().item(),
        "actor/ppo_kl": ppo_kl.detach().item(),
    }
    return pg_loss, pg_metrics
