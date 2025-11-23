import torch
import torch.nn as nn
from dataclasses import dataclass
from typing import Any, Dict, List, Optional, Tuple

from unsloth import FastLanguageModel


# ---------- Configs ----------

@dataclass
class ModelConfig:
    model_name: str = "Qwen/Qwen2.5-Coder-1.5B-Instruct"
    max_seq_length: int = 4096


@dataclass
class ActorConfig:
    clip_ratio: float = 0.2
    clip_ratio_low: Optional[float] = None
    clip_ratio_high: Optional[float] = None


@dataclass
class TrainerConfig:
    model: ModelConfig
    actor: ActorConfig
    epochs: int = 1
    max_turns: int = 4
    parallel_trajectories: int = 2

    # sampling/generation
    max_new_tokens: int = 256
    temperature: float = 0.7
    top_p: float = 0.9

    # training
    lr: float = 1e-5


# ---------- Helper loss utilities  ----------

class VerlF:
    @staticmethod
    def masked_mean(x: torch.Tensor, mask: torch.Tensor) -> torch.Tensor:
        # mask expected as float or bool with same shape as x
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
        seq_mask = torch.sum(loss_mask, dim=-1)  # per-sequence token count
        seq_losses = torch.sum(loss_mat * loss_mask, dim=-1) / (seq_mask + 1e-8)  # token-mean
        seq_mask = (seq_mask > 0).float()  # exclude fully masked sequences
        loss = verl_F.masked_mean(seq_losses, seq_mask)  # seq-mean
        return loss


def compute_policy_loss_gspo(
    old_log_prob: torch.Tensor,
    log_prob: torch.Tensor,
    advantages: torch.Tensor,
    response_mask: torch.Tensor,
    loss_agg_mode: str = "seq-mean-token-mean",
    config: Optional[ActorConfig] = None,
    rollout_is_weights: torch.Tensor | None = None,
) -> Tuple[torch.Tensor, Dict[str, Any]]:
    """
    GSPO policy loss (slightly adapted to run stand-alone).
    old_log_prob, log_prob, advantages, response_mask: (B, T)
    """
    assert config is not None
    assert isinstance(config, ActorConfig)

    clip_ratio_low = config.clip_ratio_low if config.clip_ratio_low is not None else config.clip_ratio
    clip_ratio_high = config.clip_ratio_high if config.clip_ratio_high is not None else config.clip_ratio

    # token-level log ratio
    negative_approx_kl = log_prob - old_log_prob  # (B, T)

    # sequence-average KL for GSPO exponent
    seq_lengths = response_mask.sum(dim=-1).clamp(min=1.0)  # (B,)
    negative_approx_kl_seq = (negative_approx_kl * response_mask).sum(dim=-1) / seq_lengths  # (B,)

    # log s_i,t(θ) = sg[log s_i(θ)] + log_prob - sg[log_prob]
    log_seq_importance_ratio = (
        log_prob
        - log_prob.detach()
        + negative_approx_kl_seq.detach().unsqueeze(-1)
    )
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

    pg_clipfrac = verl_F.masked_mean(
        (pg_losses2 > pg_losses1).float(),
        response_mask,
    )
    pg_clipfrac_lower = torch.tensor(0.0, device=pg_loss.device)

    ppo_kl = verl_F.masked_mean(-negative_approx_kl, response_mask)

    pg_metrics = {
        "actor/pg_clipfrac": pg_clipfrac.detach().item(),
        "actor/ppo_kl": ppo_kl.detach().item(),
        "actor/pg_clipfrac_lower": pg_clipfrac_lower.detach().item(),
    }
    return pg_loss, pg_metrics


# ---------- Trainer ----------

class MultiTurnRLTrainer:
    def __init__(
        self,
        config: TrainerConfig,
        verify_fn,
        profile_fn,
        reward_fn,
        construct_prompt_fn,
        parse_response_fn,
    ):
        self.config = config

        # Initialize model and tokenizer in HF mode (no fast_inference)
        model_name = self.config.model.model_name
        self.model, self.tokenizer = FastLanguageModel.from_pretrained(
            model_name,
            max_seq_length=self.config.model.max_seq_length,
            load_in_4bit=False,
            dtype=torch.bfloat16,
            fast_inference=False,  # IMPORTANT: pure HF path
        )
        self.model = self.model.to("cuda" if torch.cuda.is_available() else "cpu")
        self.device = next(self.model.parameters()).device

        self.max_turns = config.max_turns
        self.parallel_trajectories = config.parallel_trajectories

        self.verify_fn = verify_fn
        self.profile_fn = profile_fn
        self.construct_prompt_fn = construct_prompt_fn
        self.parse_response_fn = parse_response_fn
        self.reward_fn = reward_fn

        self.actor_config = config.actor
        self.optimizer = torch.optim.AdamW(self.model.parameters(), lr=config.lr)

    # ---------- Public API ----------

    def train(self, dataloader):
        for epoch in range(self.config.epochs):
            for batch in dataloader:
                # batch: list[task dict]
                trajectories = self.rollout(batch)
                loss, metrics = self.update_policy(trajectories)
                print(
                    f"[epoch {epoch}] loss={loss:.4f}, "
                    f"clipfrac={metrics['actor/pg_clipfrac']:.4f}, "
                    f"kl={metrics['actor/ppo_kl']:.4f}"
                )

    # ---------- Rollout (batched HF generate) ----------

    def rollout(self, batch: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """
        batch: list of tasks, each:
            {
              "task_id": ..,
              "prompt": str,
              "solution_scalar": ...,
              ...
            }
        returns: list of trajectories, each:
            {
              "task_id": ...,
              "turns": [
                 {
                   "turn": int,
                   "prompt": str,
                   "response": str,
                   "reward": float,
                   "token_log_probs_old": (T,),
                   "input_ids_full": (L,),
                   "prompt_len": int,
                   "response_len": int,
                 }, ...
              ]
            }
        """
        # envs = task × parallel_trajectories
        envs = []
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

        for turn in range(self.max_turns):
            prompts = [
                self.construct_prompt_fn(env["context_buffer"], turn) for env in envs
            ]

            gen_infos = self._batched_generate(prompts)

            for env, prompt, gen_info in zip(envs, prompts, gen_infos):
                response = gen_info["response_text"]
                token_log_probs_old = gen_info["token_log_probs_old"]
                input_ids_full = gen_info["input_ids_full"]
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

        trajectories = [env["trajectory"] for env in envs]
        return trajectories

    def _batched_generate(self, prompts: List[str]) -> List[Dict[str, Any]]:
        """
        Use HF generate with output_scores=True to get per-token logprobs.
        Returns one dict per prompt:
            {
              "response_text": str,
              "token_log_probs_old": (T,),
              "input_ids_full": (L,),
              "prompt_len": int,
              "response_len": int,
            }
        """
        self.model.eval()

        # Tokenize prompts
        enc = self.tokenizer(
            prompts,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.config.model.max_seq_length,
        )
        input_ids = enc["input_ids"].to(self.device)
        attention_mask = enc["attention_mask"].to(self.device)
        input_lengths = attention_mask.sum(dim=-1)  # (B,)

        with torch.no_grad():
            gen_out = self.model.generate(
                input_ids=input_ids,
                attention_mask=attention_mask,
                max_new_tokens=self.config.max_new_tokens,
                do_sample=True,
                temperature=self.config.temperature,
                top_p=self.config.top_p,
                output_scores=True,
                return_dict_in_generate=True,
            )

        sequences = gen_out.sequences  # (B, L_total)
        scores = gen_out.scores        # list[T_gen] of (B, vocab)
        B = sequences.size(0)
        T_gen = len(scores)

        results: List[Dict[str, Any]] = []

        for i in range(B):
            prompt_len = int(input_lengths[i].item())
            full_seq = sequences[i]                     # (L_total,)
            gen_tokens = full_seq[prompt_len:]          # response part
            response_len = gen_tokens.size(0)

            # Compute per-token logprobs for generated tokens
            token_log_probs = []
            for t in range(response_len):
                if t >= T_gen:
                    break
                logits = scores[t][i]                   # (vocab,)
                logprobs = torch.log_softmax(logits, dim=-1)
                tok_id = gen_tokens[t]
                token_log_probs.append(logprobs[tok_id])

            token_log_probs_old = torch.stack(token_log_probs)  # (T,)
            response_text = self.tokenizer.decode(
                gen_tokens,
                skip_special_tokens=True,
            )

            results.append(
                {
                    "response_text": response_text,
                    "token_log_probs_old": token_log_probs_old.detach().cpu(),
                    "input_ids_full": full_seq.detach().cpu(),
                    "prompt_len": prompt_len,
                    "response_len": response_len,
                }
            )

        return results

    # ---------- Evaluation helpers ----------

    def evaluate_response(self, response: str, turn: int):
        verification = self.verify_fn(response, turn)
        if verification.get("all", False):
            profile = self.profile_fn(response, turn)
        else:
            profile = None
        return verification, profile

    # ---------- Policy update / GSPO ----------

    def update_policy(self, trajectories: List[Dict[str, Any]]):
        turns = self._flatten_trajectories(trajectories)
        if len(turns) == 0:
            return 0.0, {"actor/pg_clipfrac": 0.0, "actor/ppo_kl": 0.0, "actor/pg_clipfrac_lower": 0.0}

        (
            input_ids,
            attention_mask,
            prompt_lens,
            response_lens,
            old_log_prob,
            response_mask,
            advantages,
        ) = self._collate_batch_for_gspo(turns)

        max_T = old_log_prob.size(1)

        # Current-policy log_probs (requires grad)
        log_prob, response_mask_new = self._compute_log_probs_current_policy(
            input_ids, attention_mask, prompt_lens, response_lens, max_T
        )

        final_mask = response_mask * response_mask_new

        pg_loss, pg_metrics = compute_policy_loss_gspo(
            old_log_prob=old_log_prob,
            log_prob=log_prob,
            advantages=advantages,
            response_mask=final_mask,
            loss_agg_mode="seq-mean-token-mean",
            config=self.actor_config,
            rollout_is_weights=None,
        )

        self.optimizer.zero_grad()
        pg_loss.backward()
        self.optimizer.step()

        return float(pg_loss.detach().item()), pg_metrics

    def _flatten_trajectories(self, trajectories: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        turns: List[Dict[str, Any]] = []
        for traj in trajectories:
            for turn in traj["turns"]:
                turns.append(turn)
        return turns

    def _collate_batch_for_gspo(
        self, turns: List[Dict[str, Any]]
    ):
        """
        turns[i]:
          {
            "prompt": str,
            "response": str,
            "reward": float,
            "token_log_probs_old": (T,),
            "input_ids_full": (L,),
            "prompt_len": int,
            "response_len": int,
          }
        """
        B = len(turns)
        lengths = [int(t["response_len"]) for t in turns]
        max_T = max(lengths)

        old_log_prob = torch.zeros(B, max_T, device=self.device)
        response_mask = torch.zeros(B, max_T, device=self.device)
        rewards = torch.zeros(B, device=self.device)

        input_ids_list = []
        prompt_lens = []
        response_lens = []

        for i, t in enumerate(turns):
            lp = t["token_log_probs_old"].to(self.device)  # (T,)
            T = lp.size(0)
            old_log_prob[i, :T] = lp
            response_mask[i, :T] = 1.0
            rewards[i] = float(t["reward"])

            ids_full = t["input_ids_full"].to(self.device)
            input_ids_list.append(ids_full)
            prompt_lens.append(int(t["prompt_len"]))
            response_lens.append(T)

        # Pad full sequences for a single forward pass
        pad_token_id = self.tokenizer.pad_token_id or 0
        input_ids = nn.utils.rnn.pad_sequence(
            input_ids_list,
            batch_first=True,
            padding_value=pad_token_id,
        )
        attention_mask = (input_ids != pad_token_id).long()

        input_ids = input_ids.to(self.device)
        attention_mask = attention_mask.to(self.device)
        prompt_lens = torch.tensor(prompt_lens, device=self.device, dtype=torch.long)
        response_lens = torch.tensor(response_lens, device=self.device, dtype=torch.long)

        # Simple advantage: reward broadcast to all response tokens
        advantages = rewards.unsqueeze(1) * response_mask  # (B, T)

        return (
            input_ids,
            attention_mask,
            prompt_lens,
            response_lens,
            old_log_prob,
            response_mask,
            advantages,
        )

    def _compute_log_probs_current_policy(
        self,
        input_ids: torch.Tensor,
        attention_mask: torch.Tensor,
        prompt_lens: torch.Tensor,
        response_lens: torch.Tensor,
        max_T: int,
    ):
        """
        Compute token-level log_probs for response tokens under current policy.
        Returns:
          log_prob: (B, max_T)
          response_mask: (B, max_T)
        """
        self.model.train()  # we are in training phase

        outputs = self.model(input_ids=input_ids, attention_mask=attention_mask)
        logits = outputs.logits  # (B, L, V)
        logprobs_all = torch.log_softmax(logits, dim=-1)

        B, L, V = logprobs_all.shape
        log_prob = torch.zeros(B, max_T, device=self.device)
        response_mask = torch.zeros(B, max_T, device=self.device)

        for i in range(B):
            prompt_len = int(prompt_lens[i].item())
            T = int(response_lens[i].item())
            T = min(T, max_T)

            for t in range(T):
                pos = prompt_len + t
                if pos <= 0 or pos >= L:
                    break
                tok_id = input_ids[i, pos]
                # next-token LM: token at pos has logprob from logits at pos-1
                log_prob[i, t] = logprobs_all[i, pos - 1, tok_id]
                response_mask[i, t] = 1.0

        return log_prob, response_mask
