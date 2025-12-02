# Wraps model into policy
# TODO: install unsloth vllm
import os
#os.environ["UNSLOTH_VLLM_STANDBY"] = "1" # unsloth optimization
import unsloth
import torch
from vllm import SamplingParams
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
            gpu_memory_utilization = 0.6,
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

        sampling_params = SamplingParams(
            temperature=kwargs.get("temperature", 0.7),
            max_tokens=kwargs.get("max_new_tokens", 512),
        )

        # prepare batch
        # inputs = self.tokenizer(
        #     prompts,
        #     return_tensors="pt",
        #     padding=True,
        #     truncation=True,
        #     max_length=self.max_seq_length
        # ).to(self.device)

        # generate responses
        # with torch.no_grad():
        with torch.inference_mode():
            # outputs = self.model.generate(
            #     **inputs,
            #     max_new_tokens=kwargs.get("max_new_tokens", 512),
            #     do_sample=kwargs.get("do_sample", True),
            #     temperature=kwargs.get("temperature", 0.7),
            #     use_cache=True,
            #     pad_token_id=self.tokenizer.pad_token_id,
            #     eos_token_id=self.tokenizer.eos_token_id,
            # )
            outputs = self.model.fast_generate(
                prompts,
                sampling_params=sampling_params
            )

        # old
        # input_tensor_width = inputs.input_ids.shape[1]
        # completions = []
        # for i, output_ids in enumerate(outputs):
        #     # Slice using the full width
        #     generated_ids = output_ids[input_tensor_width:]

        #     completion = self.tokenizer.decode(generated_ids, skip_special_tokens=True)
        #     completions.append(completion)

        # new
        completions = []
        for request_output in outputs:
            generated_text = request_output.outputs[0].text
            completions.append(generated_text)


        # 🔍 DEBUG LOGGING HERE
        if self.debug:
            print("\n[UnifiedPolicyEngine.generate] Debug samples:")
            for i, (p, full, c) in enumerate(zip(prompts, outputs, completions)):
                print(f"\n--- Sample {i} ---")
                print("PROMPT:\n", p)
                print("\nFULL MODEL OUTPUT:\n", full)
                print("\nCOMPLETION (after stripping prompt):\n", c, "...")

        torch.cuda.empty_cache()
        return completions

    def generate_log_probs(
        self,
        prompts: list[str],
        responses: list[str],
        micro_batch_size: int = 2 # Default to small batch
    ) -> tuple[torch.Tensor, torch.Tensor]:
        """
        Computes log probabilities in micro-batches to prevent OOM.
        """
        # Training/Forward pass requires RIGHT padding
        self.tokenizer.padding_side = "right"

        # 1. Prepare full texts
        full_responses = [r + self.tokenizer.eos_token for r in responses]
        full_texts = [p + r for p, r in zip(prompts, full_responses)]
        
        # Lists to store results from each micro-batch
        all_log_probs = []
        all_masks = []

        # 2. Iterate in chunks (Micro-Batching)
        total_len = len(full_texts)
        for i in range(0, total_len, micro_batch_size):
            # Slice the batch
            batch_texts = full_texts[i : i + micro_batch_size]
            batch_prompts = prompts[i : i + micro_batch_size]

            # Tokenize ONLY this small chunk
            inputs = self.tokenizer(
                batch_texts,
                add_special_tokens=False,
                return_tensors="pt",
                padding=True,
                truncation=True,
                max_length=self.max_seq_length,
            ).to(self.device)

            input_ids = inputs.input_ids
            attention_mask = inputs.attention_mask

            # Get prompt lengths for this chunk
            prompt_inputs = self.tokenizer(
                batch_prompts,
                add_special_tokens=False,
                return_tensors="pt",
                padding=True,
                truncation=True,
                max_length=self.max_seq_length
            )
            prompt_lens = prompt_inputs.attention_mask.sum(dim=1).to(self.device)

            # --- Forward pass (Activations generated here) ---
            outputs = self.model(input_ids=input_ids, attention_mask=attention_mask)
            logits = outputs.logits[:, :-1, :]
            target_ids = input_ids[:, 1:]

            # Calculate Loss (Log Probs)
            loss_fct = torch.nn.CrossEntropyLoss(reduction='none')
            token_log_probs = -loss_fct(
                logits.reshape(-1, logits.size(-1)),
                target_ids.reshape(-1)
            ).view(target_ids.shape)

            # Create Mask
            mask = torch.zeros_like(token_log_probs)
            for j, p_len in enumerate(prompt_lens):
                start_idx = max(p_len - 1, 0)
                mask[j, start_idx:] = 1.0
            
            padding_mask = attention_mask[:, 1:]
            final_mask = mask * padding_mask
            
            # Apply mask
            token_log_probs = token_log_probs * final_mask

            # Store results
            all_log_probs.append(token_log_probs)
            all_masks.append(final_mask)

            # --- CRITICAL: Clean up memory immediately ---
            del inputs, outputs, logits, token_log_probs, final_mask
            torch.cuda.empty_cache()

        # 3. Concatenate all micro-batches
        # We must pad them to the same width (sequence length) before stacking
        max_seq_len = max(t.size(1) for t in all_log_probs)
        
        padded_log_probs = []
        padded_masks = []
        
        for lp, m in zip(all_log_probs, all_masks):
            pad_len = max_seq_len - lp.size(1)
            if pad_len > 0:
                lp = torch.nn.functional.pad(lp, (0, pad_len), value=0.0)
                m = torch.nn.functional.pad(m, (0, pad_len), value=0.0)
            padded_log_probs.append(lp)
            padded_masks.append(m)

        return torch.cat(padded_log_probs, dim=0), torch.cat(padded_masks, dim=0)

    def update_training_engine(self, loss) -> tuple[float, float]:
        """
        Performs a single gradient update step.
        """
        # 1. Backward pass
        self.optimizer.zero_grad()
        loss.backward()
        
        # 2. Gradient clipping & Norm tracking
        # clip_grad_norm_ returns the total norm of the parameters (tensor)
        # We capture this for your "collapse detection" metric
        grad_norm_tensor = torch.nn.utils.clip_grad_norm_(self.model.parameters(), max_norm=1.0)
        grad_norm = grad_norm_tensor.item()

        # 3. Optimizer step
        self.optimizer.step()

        return loss.item(), grad_norm

    def set_train_mode(self):
        FastLanguageModel.for_training(self.model)
        self.model.train()

    def save_model(self, output_dir: str):
        self.model.save_pretrained(output_dir)
        self.tokenizer.save_pretrained(output_dir)
