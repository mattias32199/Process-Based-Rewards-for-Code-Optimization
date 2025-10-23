import tqdm
import os
import sys
from contextlib import redirect_stdout
import io
import fire
from simdbench.data import SIMD_BENCH,simdbench_scalar
from simdbench.evaluation import evaluate_performance

simdbench_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "../")
# performance_output = os.path.join(simdbench_path , "results/performance-eval")
model_lst = ["claude-3-5-haiku-20241022", "claude-3-5-sonnet-20241022", "codestral-latest", 
             "deepseek-chat", "deepseek-reasoner", "gemini-2.0-flash", "gemini-2.5-flash-preview-05-20", 
             "gpt-3.5-turbo", "gpt-4o-2024-11-20", "gpt-4o-mini-2024-07-18", 
             "grok-3", "grok-3-beta", "mistral-large-latest", "qwen-coder-plus", 
             "qwen-coder-turbo", "qwen-max-2025-01-25", "qwen-plus-2025-01-25", "qwen-turbo-2025-02-11"]

# model_lst = ["claude-3-5-haiku-20241022"] # for test

def entry_point(performance_output:str, intrinsic:str = "AVX"):
    # intrinsic in ["SSE", "AVX", "Neon", "SVE", "RVV"]
    #check model output
    print("checking model output...")
    for model in model_lst:
        path = f"results/model-output/{model}/{model}-{intrinsic}.jsonl"
        model_output = os.path.join(simdbench_path, path)
        assert( os.path.exists(model_output) )

    for model in tqdm.tqdm(model_lst, desc="Evaluating models", leave=False):
        path = f"results/model-output/{model}/{model}-{intrinsic}.jsonl"
        model_output = os.path.join(simdbench_path, path)
        model_output = os.path.abspath(model_output)
                
        # entry point
        sample_file = model_output
        k = "1,5"
        k = list(map(int, k.split(",")))
        n_workers = os.cpu_count()
        timeout = 30.0
        problem_file = SIMD_BENCH
        if(intrinsic=="scalar" and problem_file == SIMD_BENCH): problem_file = simdbench_scalar
        n_reputation: int = 5
        output_path = performance_output
        
        with redirect_stdout(io.StringIO()):
            evaluate_performance(sample_file, intrinsic, k, n_workers, timeout * n_reputation, problem_file, "-O0", n_reputation, output_path)
            evaluate_performance(sample_file, intrinsic, k, n_workers, timeout * n_reputation, problem_file, "-O3", n_reputation, output_path)

def main():
    fire.Fire(entry_point)

sys.exit(main())