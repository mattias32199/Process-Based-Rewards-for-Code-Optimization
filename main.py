import torch
from typing import Dict, Any

# Import your modules
# (Assuming files are in the same directory. If in 'src', use 'from src.config import ...')
from src.config import ModelConfig, ActorConfig, TrainerConfig
from src.engine import UnifiedPolicyEngine
from src.trainer import MultiTurnRLTrainer

# ---------- 1. Define Domain Logic ----------

def construct_prompt_fn(context_buffer: Dict[str, Any], turn: int) -> str:
    """
    Creates the input prompt.
    """
    task = context_buffer["task"]
    prompt_text = task["prompt"]
    
    # Simple Chat Template
    full_prompt = (
        f"<|im_start|>system\nYou are a helpful math assistant. "
        f"Think step by step and output the final answer after 'Solution:'.<|im_end|>\n"
        f"<|im_start|>user\n{prompt_text}<|im_end|>\n"
        f"<|im_start|>assistant\n"
    )
    return full_prompt

def parse_response_fn(response_text: str):
    """
    Extracts Chain-of-Thought (CoT) and the specific Answer.
    """
    # Simple parsing: assumes the model says "Solution: <answer>"
    if "Solution:" in response_text:
        parts = response_text.split("Solution:")
        cot = parts[0].strip()
        solution = parts[1].strip()
    else:
        cot = response_text
        solution = "" # Failed to produce structured answer
    return cot, solution

def verify_fn(response_text: str, turn: int):
    """
    Returns metadata about correctness.
    We don't calculate reward here, just verification status.
    """
    # In a real app, you might compare 'solution' vs 'ground_truth'
    # Here, we will do that in the reward_fn for simplicity, 
    # or pass data through via a 'correct' flag.
    return {"all": True} # 'all' triggers the profile_fn

def profile_fn(response_text: str, turn: int):
    """
    Optional logging stats (length, latency, etc.)
    """
    return {"len": len(response_text)}

def reward_fn(verification, profile, turn, context_buffer):
    """
    Calculates the actual scalar reward.
    Note: We need to modify the Trainer's signature slightly if we want 
    access to 'context_buffer' inside reward_fn, but for this demo, 
    we will hack it: the Trainer passed (verification, profile, turn).
    
    *Correction*: Your Trainer class usually passes (verification, profile, turn).
    To check ground truth, we usually bake ground truth checking into 'verify_fn'.
    """
    # Let's assume we implement the ground truth check inside verify_fn in a real scenario.
    # For this dummy test, let's just reward LENGTH to see if it learns to yap 
    # (The "Hello World" of RL) OR check for a specific keyword.
    
    # "Real" Logic Simulation:
    if "42" in verification.get("response_text", ""): # Hack for demo
        return 1.0
    
    # Fallback "Hello World" Reward: Penalize being empty
    if profile["len"] < 5:
        return -1.0
        
    return 0.1 # Small positive reward for generating anything

# Wrapper to fit the Trainer signature exactly
def reward_fn_wrapper(verification, profile, turn):
    # In a real implementation, 'verification' would contain the boolean 'is_correct'
    return 1.0 if verification.get("is_correct", False) else 0.0

def verify_fn_real(response_text, turn, ground_truth):
    is_correct = ground_truth in response_text
    return {"is_correct": is_correct, "all": True}


# ---------- 2. The Main Execution Block ----------

if __name__ == "__main__":
    # A. Configuration
    # Use a small model and short sequence for quick testing
    model_cfg = ModelConfig(
        model_name="Qwen/Qwen2.5-Coder-1.5B-Instruct",
        max_seq_length=1024
    )
    
    actor_cfg = ActorConfig(
        clip_ratio=0.2
    )
    
    trainer_cfg = TrainerConfig(
        model=model_cfg,
        actor=actor_cfg,
        epochs=1,                # 1 Epoch for testing
        max_turns=1,             # Single turn per math problem
        parallel_trajectories=2, # Batch size of 2
        max_new_tokens=64,       # Short generation
        temperature=0.8,
        lr=5e-5
    )

    # B. Initialize Engine (GPU)
    print(">>> Initializing Engine...")
    engine = UnifiedPolicyEngine(
        model_name=model_cfg.model_name,
        max_seq_length=model_cfg.max_seq_length,
        learning_rate=trainer_cfg.lr,
        fast_inference=True # ENABLE optimization
    )

    # C. Initialize Trainer
    # We need to curry/partial the functions if they need extra data (like ground truth)
    # But for this simple test, we will assume the environment handles it or use simple closures.
    
    # Creating a closure for verify_fn to handle ground truth is tricky in a batch list.
    # Instead, we will look at how your Trainer 'evaluate_response' works.
    # It passes (response, turn).
    # We will assume a simple "Length Reward" for this pure pipeline test 
    # to avoid complex prompt/answer matching logic in the main file.
    
    def simple_reward(verification, profile, turn):
        # Reward the model for outputting "Solution:"
        if verification.get("has_format"):
            return 1.0
        return -0.1

    def simple_verify(response, turn):
        return {"has_format": "Solution:" in response, "all": True}

    print(">>> Initializing Trainer...")
    trainer = MultiTurnRLTrainer(
        config=trainer_cfg,
        engine=engine,
        verify_fn=simple_verify,
        profile_fn=profile_fn,
        reward_fn=simple_reward,
        construct_prompt_fn=construct_prompt_fn,
        parse_response_fn=parse_response_fn
    )

    # D. Dummy Data
    # List of tasks. The Trainer iterates over this list.
    dummy_dataloader = [
        [
            {"task_id": 1, "prompt": "What is 20 + 22?", "solution_scalar": "42"},
            {"task_id": 2, "prompt": "What is 50 - 8?", "solution_scalar": "42"},
        ],
        [
            {"task_id": 3, "prompt": "What is 6 * 7?", "solution_scalar": "42"},
            {"task_id": 4, "prompt": "What is 84 / 2?", "solution_scalar": "42"},
        ]
    ]

    # E. Run
    print(">>> Starting Training Loop...")
    trainer.train(dummy_dataloader)
    
    print(">>> Test Complete. Saving Adapter...")
    engine.model.save_pretrained("debug_output_lora")
    engine.tokenizer.save_pretrained("debug_output_lora")
    print(">>> Saved to 'debug_output_lora'")