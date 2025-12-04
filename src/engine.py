# src/engine.py
# Wraps model into policy
# TODO: install unsloth vllm
# os.environ["UNSLOTH_VLLM_STANDBY"] = "1" # unsloth optimization
import unsloth
import torch
from vllm import SamplingParams
from unsloth import FastLanguageModel
from src.config import EngineConfig, PrintConfig

class UnifiedPolicyEngine:
    def __init__(
        self,
        config: EngineConfig,
        print_config: PrintConfig
    ):
        self.config = config
        self.print_config = print_config
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
            gpu_memory_utilization = config.gpu_memory_utilization, # 0.95, 0.6
            # enable_sleep_mode=config.vllm_sleep,
            fast_inference=True
        )

        # if self.debug:
        #     print(f"[DEBUG] EOS token: {self.tokenizer.eos_token}")
        #     print(f"[DEBUG] PAD token: {self.tokenizer.pad_token}")
        #     print(f"[DEBUG] Special tokens: {self.tokenizer.special_tokens_map}")
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
        if self.print_config.prompts_and_completions:
            print("\n[UnifiedPolicyEngine.generate] Debug samples:")
            for i, (p, full, c) in enumerate(zip(prompts, outputs, completions)):
                print(f"\n--- Sample {i} ---")
                print("PROMPT:\n", p)
                print("\nFULL MODEL OUTPUT:\n", full)
                print("\nCOMPLETION (after stripping prompt):\n", c, "...")

        torch.cuda.empty_cache()
        return completions

    def generate_log_probs(self,prompts: list[str], responses: list[str], micro_batch_size:int=8) -> tuple[torch.Tensor, torch.Tensor]:
        """
        Computes log probabilities for the 'responses' given the 'prompts'.
        Used for both:
        1. Calculating 'old_log_probs' (no_grad) during Rollout.
        2. Calculating 'new_log_probs' (grad) during Update.

        Returns:
            log_probs: (B, T) - Log probs of the response tokens
            masks: (B, T) - 1.0 for response tokens, 0.0 for padding/prompt
        """
        # Setup
        self.tokenizer.padding_side = "right" # training/forward pass requires RIGHT padding
        responses = [r + self.tokenizer.eos_token for r in responses] # append EOS
        full_texts = [p + r for p, r in zip(prompts, responses)] # construct full texts

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
        prompt_inputs = self.tokenizer(
            prompts,
            add_special_tokens=False,
            return_tensors="pt",
            padding=True,
            truncation=True,
            max_length=self.max_seq_length
        )
        prompt_lens = prompt_inputs.attention_mask.sum(dim=1).to(self.device) # calculate prompt lengths

        all_log_probs = []
        all_masks = []
        num_samples = input_ids.size(0)
        for i in range(0, num_samples, micro_batch_size):
            # Define slice
            end = min(i + micro_batch_size, num_samples)
            batch_slice = slice(i, end)

            curr_input_ids = input_ids[batch_slice]
            curr_attn_mask = attention_mask[batch_slice]
            curr_prompt_lens = prompt_lens[batch_slice]

            outputs = self.model(
                input_ids=curr_input_ids,
                attention_mask=curr_attn_mask,
                use_cache=False
            )
            logits = outputs.logits

            shift_logits = logits[..., :-1, :].contiguous()
            shift_labels = curr_input_ids[..., 1:].contiguous()

            # loss
            loss_fct = torch.nn.CrossEntropyLoss(reduction='none')
            log_probs = -loss_fct(
                shift_logits.view(-1, shift_logits.size(-1)),
                shift_labels.view(-1)
            )

            log_probs = log_probs.view(shift_labels.shape) # Reshape back to [MicroBatch, SeqLen]

            # immediate cleanup
            del logits, shift_logits, outputs
            torch.cuda.empty_cache()

            # masking logic
            mask = torch.zeros_like(log_probs)
            for j, p_len in enumerate(curr_prompt_lens):
                start_idx = max(p_len - 1, 0)
                mask[j, start_idx:] = 1.0
            padding_mask = curr_attn_mask[:, 1:]
            final_mask = mask * padding_mask
            log_probs = log_probs * final_mask
            all_log_probs.append(log_probs)
            all_masks.append(final_mask)

        # re-assembly
        token_log_probs = torch.cat(all_log_probs, dim=0)
        final_mask = torch.cat(all_masks, dim=0)

        if self.print_config.log_probs:
            with torch.no_grad():
                mask_counts = final_mask.sum(dim=1)
                print("\n[get_batch_log_probs] batch size:", final_mask.size(0))
                print("[get_batch_log_probs] seq len:", final_mask.size(1))
                print("[get_batch_log_probs] nonzero mask per sample:", mask_counts.tolist())
                print("[get_batch_log_probs] total nonzero mask:", mask_counts.sum().item())
                print("[get_batch_log_probs] token_logprobs_all mean:",
                    float(token_log_probs.mean().item()))
                print("[get_batch_log_probs] token_log_probs (after mask) mean:",
                    float(token_log_probs.mean().item()))

        return token_log_probs, final_mask

    def step_training_engine(self):
        """
        Performs a single gradient update step.
        Args:
            loss
        Returns:
            loss: float (The training loss for logging)
        """
        # gradient clipping
        torch.nn.utils.clip_grad_norm_(self.model.parameters(), max_norm=1.0)
        self.optimizer.step()
        # return loss.item()

    def set_train_mode(self):
        FastLanguageModel.for_training(self.model)
        self.model.train()

    def save_model(self, output_dir: str):
        self.model.save_pretrained(output_dir)
        self.tokenizer.save_pretrained(output_dir)
