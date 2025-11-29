# Wraps model into policy
# TODO: install unsloth vllm
import os
os.environ["UNSLOTH_VLLM_STANDBY"] = "1" # unsloth optimization

import torch
from unsloth import FastLanguageModel
from src.config import EngineConfig

class UnifiedPolicyEngine:
    def __init__(
        self,
        config: EngineConfig
    ):
        self.config = config
        self.device = "cuda"
        self.learning_rate = self.config.lr
        self.max_seq_length = config.model.max_seq_length
        self.debug = config.debug

        # load model
        self.model, self.tokenizer = FastLanguageModel.from_pretrained(
            model_name=self.config.model.model_name,
            max_seq_length=self.max_seq_length,
            load_in_16bit=True,
            dtype=config.model.dtype, # load and use in fp16
            gpu_memory_utilization = 0.95,
            fast_inference=True
        )

        if self.debug:
            print(f"[DEBUG] EOS token: {self.tokenizer.eos_token}")
            print(f"[DEBUG] PAD token: {self.tokenizer.pad_token}")
            print(f"[DEBUG] Special tokens: {self.tokenizer.special_tokens_map}")
        if self.tokenizer.pad_token is None:
            self.tokenizer.pad_token = self.tokenizer.eos_token

        # LoRA adapter
        # https://thinkingmachines.ai/blog/lora/
        # https://docs.unsloth.ai/get-started/fine-tuning-llms-guide/lora-hyperparameters-guide
        """
        - RL requires low capacity
        - Target all layers
        - 10x learning rate of full ft
        """
        self.model = FastLanguageModel.get_peft_model(
            self.model,
            r=config.lora.lora_rank,
            target_modules=["q_proj", "k_proj", "v_proj", "o_proj",
                            "gate_proj", "up_proj", "down_proj"], # target all layers
            lora_alpha=config.lora.lora_rank,
            lora_dropout=config.lora.lora_dropout,
            bias="none",
            use_gradient_checkpointing="unsloth",
            random_state=config.lora.random_state,
        )

        # 3. optimizer
        self.optimizer = torch.optim.AdamW(
            self.model.parameters(),
            lr=self.learning_rate,
            weight_decay=self.config.weight_decay
        )

        # ensure pad token
        if self.tokenizer.pad_token is None:
            self.tokenizer.pad_token = self.tokenizer.eos_token

    def generate_responses(self, prompts: list[str], **kwargs) -> list[str]:
        """
        Switches to Inference Mode and generates responses for a batch of prompts.
        """
        # 1. Switch to optimized inference kernels (FlashAttn 2, no gradient tracking)
        FastLanguageModel.for_inference(self.model)
        self.model.eval()
        # Inference requires left padding
        self.tokenizer.padding_side = "left"
        torch.cuda.empty_cache() # clear cache

        # prepare batch
        inputs = self.tokenizer(
            prompts,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_seq_length
        ).to(self.device)

        # generate responses
        # with torch.no_grad():
        with torch.inference_mode():
            outputs = self.model.generate(
                **inputs,
                max_new_tokens=kwargs.get("max_new_tokens", 512),
                do_sample=kwargs.get("do_sample", True),
                temperature=kwargs.get("temperature", 0.7),
                use_cache=True,
                pad_token_id=self.tokenizer.pad_token_id,
                eos_token_id=self.tokenizer.eos_token_id,
            )

        input_tensor_width = inputs.input_ids.shape[1]

        completions = []
        for i, output_ids in enumerate(outputs):
            # Slice using the full width
            generated_ids = output_ids[input_tensor_width:]

            completion = self.tokenizer.decode(generated_ids, skip_special_tokens=True)
            completions.append(completion)
        del outputs, inputs
        torch.cuda.empty_cache()

        # 🔍 DEBUG LOGGING HERE
        if self.debug:
            print("\n[UnifiedPolicyEngine.generate] Debug samples:")
            full_decoded = self.tokenizer.batch_decode(outputs, skip_special_tokens=False)
            for i, (p, full, c) in enumerate(zip(prompts, full_decoded, completions)):
                print(f"\n--- Sample {i} ---")
                print("PROMPT:\n", p)
                print("\nFULL MODEL OUTPUT:\n", full)
                print("\nCOMPLETION (after stripping prompt):\n", c, "...")

        return completions

    def generate_log_probs(
        self,
        prompts: list[str],
        responses: list[str]
    ) -> tuple[torch.Tensor, torch.Tensor]:
        """
        Computes log probabilities for the 'responses' given the 'prompts'.
        Used for both:
        1. Calculating 'old_log_probs' (no_grad) during Rollout.
        2. Calculating 'new_log_probs' (grad) during Update.

        Returns:
            log_probs: (B, T) - Log probs of the response tokens
            masks: (B, T) - 1.0 for response tokens, 0.0 for padding/prompt
        """
        # Training/Forward pass requires RIGHT padding
        self.tokenizer.padding_side = "right"

        # We need to construct the full text to feed into the model
        # Note: Ideally you use a chat template here, ensuring strict concatenation
        full_texts = [p + r for p, r in zip(prompts, responses)]

        # Tokenize everything
        inputs = self.tokenizer(
            full_texts,
            add_special_tokens=False,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_seq_length,
        ).to(self.device)

        input_ids = inputs.input_ids
        attention_mask = inputs.attention_mask

        # Determine where the prompt ends and response begins
        # We re-tokenize just the prompts to find their length
        prompt_inputs = self.tokenizer(
            prompts,
            add_special_tokens=False,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_seq_length
        )
        # Calculate prompt lengths
        prompt_lens = prompt_inputs.attention_mask.sum(dim=1).to(self.device)

        # Forward pass
        outputs = self.model(input_ids=input_ids, attention_mask=attention_mask)
        logits = outputs.logits # (B, Seq, Vocab)

        # Shift: logits at [:-1] predict tokens at [1:]
        logits = logits[:, :-1, :]
        target_ids = input_ids[:, 1:]

        # Compute Log Softmax
        log_probs_all = torch.log_softmax(logits, dim=-1)

        # Gather log prob of the actual target tokens
        token_log_probs_all = torch.gather(
            log_probs_all,
            dim=-1,
            index=target_ids.unsqueeze(-1)
        ).squeeze(-1) # (B, Seq-1)

        # bos agnostic
        mask = torch.zeros_like(token_log_probs_all)
        for i, p_len in enumerate(prompt_lens):
            start_idx = p_len - 1
            start_idx = max(start_idx, 0)
            mask[i, start_idx:] = 1.0

        # Apply padding mask (original attention mask shifted)
        # attention_mask is for [0...Seq]. target_ids is [1...Seq]
        padding_mask = attention_mask[:, 1:]
        final_mask = mask * padding_mask

        # Zero out logprobs for prompt/padding so they don't affect sums
        token_log_probs = token_log_probs_all * final_mask

        if self.debug:
            with torch.no_grad():
                mask_counts = final_mask.sum(dim=1)
                print("\n[get_batch_log_probs] batch size:", final_mask.size(0))
                print("[get_batch_log_probs] seq len:", final_mask.size(1))
                print("[get_batch_log_probs] nonzero mask per sample:", mask_counts.tolist())
                print("[get_batch_log_probs] total nonzero mask:", mask_counts.sum().item())
                print("[get_batch_log_probs] token_logprobs_all mean:",
                    float(token_log_probs_all.mean().item()))
                print("[get_batch_log_probs] token_log_probs (after mask) mean:",
                    float(token_log_probs.mean().item()))

        return token_log_probs, final_mask

    def update_training_engine(self, loss) -> float:
        """
        Performs a single gradient update step.
        Args:
            loss
        Returns:
            loss: float (The training loss for logging)
        """
        # backward pass
        self.optimizer.zero_grad()
        loss.backward()
        # gradient clipping
        torch.nn.utils.clip_grad_norm_(self.model.parameters(), max_norm=1.0)
        self.optimizer.step()

        return loss.item()

    def set_train_mode(self):
        FastLanguageModel.for_training(self.model)
        self.model.train()

    def save_model(self, output_dir: str):
        self.model.save_pretrained(output_dir)
        self.tokenizer.save_pretrained(output_dir)
