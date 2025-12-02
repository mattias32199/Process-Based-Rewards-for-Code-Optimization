# imports
from src.config import ModelConfig, LoraConfig, GSPOConfig, EngineConfig, TrainerConfig
from src.dataloader import SimdBenchDataset, SimdBenchDataLoader
# from src.engine import UnifiedPolicyEngine
from src.trainer import MultiTurnRLTrainer
from src.init_util import check_google_benchmark


if __name__ == "__main__":
    # check environment for dependencies
    # check for google benchmark
    check_google_benchmark() # throws error if google benchmark not installed

    # A. Configuration
    # Use a small model and short sequence for quick testing
    model_cfg = ModelConfig(
        model_name="Qwen/Qwen2.5-Coder-1.5B-Instruct",
        max_seq_length=2048
    )

    lora_cfg = LoraConfig(
    )

    engine_cfg = EngineConfig(
        model = model_cfg,
        lora = lora_cfg,
        debug = True,
        lr=5e-5,
        gpu_memory_utilization = 0.3
    )

    gspo_cfg = GSPOConfig(
        clip_ratio=0.2
    )

    trainer_cfg = TrainerConfig(
        engine=engine_cfg,
        gspo=gspo_cfg,
        epochs=3,                # 1 Epoch for testing
        max_turns=3,             # Single turn per math problem
        parallel_trajectories=2, # Batch size of 2
        max_new_tokens=512,       # Short generation
        temperature=0.8,
        debug=True,
        vram_verbose = True,
        save_dir="lora"
    )

    # B. Initialize Engine (GPU)
    # print(">>> Initializing Engine...")
    # engine = UnifiedPolicyEngine(
    #     model_name=model_cfg.model_name,
    #     max_seq_length=model_cfg.max_seq_length,
    #     learning_rate=trainer_cfg.lr,
    #     fast_inference=False # ENABLE optimization
    # )

    # C. Initialize Trainer
    # We need to curry/partial the functions if they need extra data (like ground truth)
    # But for this simple test, we will assume the environment handles it or use simple closures.

    # Creating a closure for verify_fn to handle ground truth is tricky in a batch list.
    # Instead, we will look at how your Trainer 'evaluate_response' works.
    # It passes (response, turn).
    # We will assume a simple "Length Reward" for this pure pipeline test
    # to avoid complex prompt/answer matching logic in the main file.

    print(">>> Initializing Trainer...")
    trainer = MultiTurnRLTrainer(
        config=trainer_cfg,
    )

    # D. Data
    dataset = SimdBenchDataset('/jet/home/jzhang73/Process-Based-Rewards-for-Code-Optimization/data/simd-all-fixed-avx.jsonl')
    dataloader = SimdBenchDataLoader(dataset, batch_size = 2)

    # E. Run
    print(">>> Starting Training Loop...")
    trainer.train(dataloader)

    print(">>> Test Complete. Saving Adapter...")
    # engine.model.save_pretrained("debug_output_lora")
    # engine.tokenizer.save_pretrained("debug_output_lora")
    print(">>> Saved to 'debug_output_lora'")
