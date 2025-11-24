import os
import torch
import torch.nn as nn
from unsloth import FastLanguageModel
from typing import List, Dict, Any, Tuple

# Enable Unsloth optimization for VLLM if installed, or general standby
os.environ["UNSLOTH_VLLM_STANDBY"] = "1"

class UnifiedPolicyEngine:
    def __init__(
        self,
        model_name: str,
        max_seq_length: int = 4096,
        lora_rank: int = 64,
        learning_rate: float = 2e-5,
        dtype=torch.float16,
    ):
        self.device = "cuda" if torch.cuda.is_available() else "cpu"
        self.max_seq_length = max_seq_length
        self.dtype = dtype

        print(f"Loading Unsloth Model: {model_name}...")
        self.model, self.tokenizer = FastLanguageModel.from_pretrained(
            model_name=model_name,
            max_seq_length=max_seq_length,
            load_in_4bit=False, # Assuming fp16 based on your snippet
            load_in_16bit=True,
            dtype=dtype,
            fast_inference=True, # Must be false to support training mode toggling
        )

        # Apply LoRA
        print(f"Applying LoRA (r={lora_rank})...")
        self.model = FastLanguageModel.get_peft_model(
            self.model,
            r=lora_rank,
            target_modules=["q_proj", "k_proj", "v_proj", "o_proj",
                            "gate_proj", "up_proj", "down_proj"],
            lora_alpha=lora_rank,
            lora_dropout=0,
            bias="none",
            use_gradient_checkpointing="unsloth",
            random_state=3407,
        )

        # Ensure Pad Token
        if self.tokenizer.pad_token is None:
            self.tokenizer.pad_token = self.tokenizer.eos_token
        
        # Optimizer
        self.optimizer = torch.optim.AdamW(
            self.model.parameters(),
            lr=learning_rate,
            weight_decay=0.01
        )

    def generate(self, prompts: List[str], **kwargs) -> List[str]:
        """
        Inference mode generation. 
        """
        FastLanguageModel.for_inference(self.model)
        self.model.eval()
        
        # Inference requires LEFT padding
        self.tokenizer.padding_side = "left"

        inputs = self.tokenizer(
            prompts,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_seq_length
        ).to(self.device)

        with torch.no_grad():
            outputs = self.model.generate(
                **inputs,
                max_new_tokens=kwargs.get("max_new_tokens", 512),
                do_sample=kwargs.get("do_sample", True),
                temperature=kwargs.get("temperature", 0.7),
                top_p=kwargs.get("top_p", 0.9),
                use_cache=True,
                pad_token_id=self.tokenizer.pad_token_id,
            )

        # Decode
        decoded = self.tokenizer.batch_decode(outputs, skip_special_tokens=True)
        
        # Simple extraction of generated part
        completions = []
        for prompt, text in zip(prompts, decoded):
            # Fallback if the tokenizer modifies the prompt slightly
            if len(text) > len(prompt):
                # Heuristic: strip the prompt. 
                # Ideally, we calculate tokens, but text matching works for simple Instruct templates
                completions.append(text[len(prompt):]) 
            else:
                completions.append("") # Generation failed or was empty

        return completions

    def get_batch_logprobs(
        self, 
        prompts: List[str], 
        responses: List[str]
    ) -> Tuple[torch.Tensor, torch.Tensor]:
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
            return_tensors="pt",
            padding=True,
            truncation=True,
            add_special_tokens=False, # Crucial: don't add extra BOS
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

        # Create the Response Mask
        # We want to mask out:
        # 1. The Prompt tokens
        # 2. The Padding tokens
        mask = torch.zeros_like(token_log_probs_all)
        
        seq_len = token_log_probs_all.size(1)
        for i in range(len(prompts)):
            p_len = prompt_lens[i].item()
            # The prompt takes up p_len tokens.
            # Because of shifting, the prediction for the first response token 
            # happens at index (p_len - 1). 
            start_idx = p_len - 1 
            if start_idx < 0: start_idx = 0
            
            mask[i, start_idx:] = 1.0

        # Apply padding mask (original attention mask shifted)
        # attention_mask is for [0...Seq]. target_ids is [1...Seq]
        padding_mask = attention_mask[:, 1:] 
        final_mask = mask * padding_mask

        # Zero out logprobs for prompt/padding so they don't affect sums
        token_log_probs = token_log_probs_all * final_mask

        return token_log_probs, final_mask

    def train_step(self, loss: torch.Tensor):
        self.optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(self.model.parameters(), max_norm=1.0)
        self.optimizer.step()
        return loss.item()

    def set_train_mode(self):
        FastLanguageModel.for_training(self.model)
        self.model.train()