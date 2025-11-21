# Wraps model into policy
# TODO: install unsloth vllm
import os
os.environ["UNSLOTH_VLLM_STANDBY"] = "1" # unsloth optimization

import torch
from unsloth import FastLanguageModel

class UnifiedPolicyEngine:
    def __init__(
        self,
        model_name: str,
        max_seq_length: int = 4096,
        lora_rank: int = 64,
        learning_rate: float = 2e-5,
        dtype=torch.float16 # https://arxiv.org/pdf/2510.26788
    ):
        self.device = "cuda"
        self.max_seq_length = max_seq_length

        # load model
        self.model, self.tokenizer = FastLanguageModel.from_pretrained(
            model_name=model_name,
            max_seq_length=max_seq_length,
            load_in_16bit=True,
            dtype=dtype, # load and use in fp16
        )

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
            r=lora_rank,
            target_modules=["q_proj", "k_proj", "v_proj", "o_proj",
                            "gate_proj", "up_proj", "down_proj"], # target all layers
            lora_alpha=lora_rank,
            lora_dropout=0,
            bias="none",
            use_gradient_checkpointing="unsloth",
            random_state=3407,
        )

        # 3. optimizer
        self.optimizer = torch.optim.AdamW(
            self.model.parameters(),
            lr=learning_rate,
            weight_decay=0.01
        )

        # ensure pad token
        if self.tokenizer.pad_token is None:
            self.tokenizer.pad_token = self.tokenizer.eos_token

    def generate_batched_responses(self, prompts: list[str], **generation_kwargs) -> list[str]:
        """
        Switches to Inference Mode and generates responses for a batch of prompts.
        """
        # switch to optimized inference kernels (FlashAttn 2, no gradient tracking)
        FastLanguageModel.for_inference(self.model)
        # switch to left padding for inference? how come?
        self.tokenizer.padding_side = "left"

        # prepare batch
        inputs = self.tokenizer(
            prompts,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_seq_length
        ).to(self.device)

        # generate responses
        outputs = self.model.generate(
            **inputs,
            max_new_tokens=generation_kwargs.get("max_new_tokens", 512),
            do_sample=generation_kwargs.get("do_sample", True),
            temperature=generation_kwargs.get("temperature", 0.7),
            use_cache=True,
            pad_token_id=self.tokenizer.pad_token_id,
            eos_token_id=self.tokenizer.eos_token_id,
        )

        # decode
        decoded = self.tokenizer.batch_decode(outputs, skip_special_tokens=True)

        # extract completions
        completions = []
        for prompt, text in zip(prompts, decoded):
            # sometimes tokenizers add a space or strip a space
            if text.startswith(prompt):
                completions.append(text[len(prompt):])
            else:
                # if prompt reconstruction fails
                completions.append(text.replace(prompt, "", 1))

        return completions

    def generate_logprobs(self, trajectories):
        """
        Calculates the length-normalized log probabilities of the response (y) given the prompt (x).
        Matches GSPO Equation 7.
        """
        # switch model to training engine
        FastLanguageModel.for_training(self.model)

        # training engine requires right padding
        self.tokenizer.padding_side = "right"

        prompts = [t['prompt'] for t in trajectories]
        completions = [t['completion'] for t in trajectories]
        full_texts = [p + c for p, c in zip(prompts, completions)]

        # Tokenize full sequences
        inputs = self.tokenizer(
            full_texts,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_seq_length
        ).to(self.device)

        # tokenize prompts separately to determine where the completion begins
        # necessary to mask out prompt logits
        prompt_inputs = self.tokenizer(
            prompts,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_seq_length,
            add_special_tokens=False # Ensure we don't double count BOS if present
        ).to(self.device)

        # lengths to create a specific completion mask
        # assumes left-padding isn't used or is handled by the tokenizer correctly.
        prompt_lengths = prompt_inputs.attention_mask.sum(dim=1)

        outputs = self.model(
            input_ids=inputs.input_ids,
            attention_mask=inputs.attention_mask
        )

        logits = outputs.logits[:, :-1]
        labels = inputs.input_ids[:, 1:]

        log_probs = torch.log_softmax(logits, dim=-1)
        token_logprobs = torch.gather(
            log_probs, -1, labels.unsqueeze(-1)
        ).squeeze(-1)

        # mask for completion tokens
        mask = torch.zeros_like(labels, dtype=torch.float)
        for i, p_len in enumerate(prompt_lengths):
            mask[i, p_len-1:] = 1.0

        # apply padding mask
        padding_mask = (labels != self.tokenizer.pad_token_id).float()
        mask = mask * padding_mask

        # calculate number of tokens in the response |y|
        response_lengths = mask.sum(dim=1).clamp(min=1)

        # xum logprobs only for the response y
        response_sum_logprobs = (token_logprobs * mask).sum(dim=1)

        # apply length normalization for GSPO (Equation 7)
        # GSPO uses the mean logprob over the sequence length
        mean_logprobs = response_sum_logprobs / response_lengths

        # check for NaN/inf
        assert torch.isfinite(mean_logprobs).all(), "Found NaN/Inf in logprobs!"

        return {
            'seq_len': response_lengths,
            'seq_logprobs': mean_logprobs # log(s_i(theta))
        }

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

    def save_model(self, output_dir: str):
        self.model.save_pretrained(output_dir)
        self.tokenizer.save_pretrained(output_dir)
