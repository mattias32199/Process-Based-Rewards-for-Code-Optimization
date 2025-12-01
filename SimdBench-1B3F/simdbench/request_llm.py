import os
import json
import tqdm
from typing import Dict
from openai import OpenAI
from tenacity import retry, stop_after_attempt, wait_random_exponential, retry_if_exception_type
from simdbench.task import sys_prompt
from simdbench.data import read_problems, simdbench_scalar, SIMD_BENCH
from simdbench.global_var import intrin_list

client = OpenAI(
    base_url="https://router.huggingface.co/v1",
    api_key=os.environ.get("HF_API_KEY"),
)

iteration = 5
model = "Qwen2.5-1.5B-Instruct"

@retry(wait=wait_random_exponential(min=1, max=60), stop=stop_after_attempt(6))
def request_llm_once(task: Dict):
    response = client.chat.completions.create(
        model=model,
        messages=[
            {"role": "system", "content": sys_prompt(task['intrinsic'], task['task'])},
            {"role": "user", "content": task['prompt']},
        ],
        temperature = 0.2,
        stream=False
    )
    return response.choices[0].message.content

# return simdbench.json
def get_data(intrinsic:str):
    assert(intrinsic in intrin_list+["scalar"])
    if(intrinsic == "scalar"): return simdbench_scalar
    elif (intrinsic in intrin_list): return SIMD_BENCH
    return None

# task range: [start_id, 135]
def samples(intrinsic:str, start_id:int = 0):
    assert(intrinsic in intrin_list+["scalar"])
    tasks = read_problems(get_data(intrinsic), intrinsic)
    with open(f"{model}-{intrinsic}.jsonl", "a", encoding="utf-8") as output:
        for task_id, task in tqdm.tqdm(tasks.items()):
            if( int(task_id.split('_')[1]) < start_id ): continue
            for _ in range(iteration):
                try:
                    completion = request_llm_once(task)
                    data = {"task_id": task_id, "completion": completion}
                    json_line = json.dumps(data)
                    output.write( json_line + '\n')
                    output.flush()
                except Exception as e:
                    print(f"[{task_id}] An error occurred: ", e)
                    pass

if __name__ == "__main__":
    # samples("scalar") # one by one
    # for item in ["SSE", "AVX", "SVE", "Neon", "RVV", "scalar"]:
    #     samples(item)
  

    for item in ["AVX"]:
        samples(item)