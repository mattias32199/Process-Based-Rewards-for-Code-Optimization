import tqdm
import os
import numpy as np
from contextlib import redirect_stdout
import io
import fire
import sys
from tabulate import tabulate

simdbench_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "../../")
sys.path.insert(0, os.path.join(simdbench_path, "src"))

from simdbench.data import SIMD_BENCH,simdbench_scalar
from simdbench.evaluation import evaluate_functional_correctness


#correctness_output = os.path.join(simdbench_path , "results/correctness-eval")

# model_lst = ["claude-3-5-haiku-20241022", "claude-3-5-sonnet-20241022", "codestral-latest", 
#              "deepseek-chat", "deepseek-reasoner", "gemini-2.0-flash", "gemini-2.5-flash-preview-05-20", 
#              "gpt-3.5-turbo", "gpt-4o-2024-11-20", "gpt-4o-mini-2024-07-18", 
#              "grok-3", "grok-3-beta", "mistral-large-latest", "qwen-coder-plus", 
#              "qwen-coder-turbo", "qwen-max-2025-01-25", "qwen-plus-2025-01-25", "qwen-turbo-2025-02-11", "qwen2.5-coder-1.5b"]

model_lst = ["claude-3-5-haiku-20241022"] # for test


code_lst = ["AVX"] 
headers = ["Model", "AVX(1)", "AVX(5)"]

pass_at_k_results = []

def entry_point(correctness_output:str):
    #check model output
    print("checking model output...")
    for model in model_lst:
        for intrinsic in code_lst:
            path = f"results/model-output/{model}/{model}-{intrinsic}.jsonl"
            model_output = os.path.join(simdbench_path, path)
            assert( os.path.exists(model_output) )

    for model in tqdm.tqdm(model_lst, desc="Evaluating models", leave=False):
        model_res = [model]
        for intrinsic in tqdm.tqdm(code_lst, desc=f"Processing {model}", leave=False):
            path = f"results/model-output/{model}/{model}-{intrinsic}.jsonl"
            model_output = os.path.join(simdbench_path, path)
            model_output = os.path.abspath(model_output)
                    
            # entry point
            sample_file = model_output
            k = "1,5"
            k = list(map(int, k.split(",")))
            n_workers = min(16, os.cpu_count())  # Reduced to avoid overloading login node
            timeout = 60.0
            problem_file = SIMD_BENCH
            if(intrinsic=="scalar" and problem_file == SIMD_BENCH): problem_file = simdbench_scalar
            output_path = correctness_output
            
            with redirect_stdout(io.StringIO()):
                pass_at_k = evaluate_functional_correctness(sample_file, intrinsic, k, n_workers, timeout, problem_file, output_path)

            pass_at_1 = np.around(pass_at_k['pass@1'] * 100, 2)
            pass_at_5 = np.around(pass_at_k['pass@5'] * 100, 2)
            model_res += [pass_at_1, pass_at_5]
        
        pass_at_k_results.append(model_res)

    print(tabulate(pass_at_k_results, headers=headers, tablefmt="grid"))

def main():
    fire.Fire(entry_point)

if __name__ == '__main__':
    sys.exit(main())