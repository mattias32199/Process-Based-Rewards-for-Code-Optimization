SimdBench
=============================
## Installation

### Get this Repository and Set Environment
Install this repository:
```bash
$ git clone TBD
$ pip install -e SimdBench
```

Set your environment:
```bash
$ export PYTHONPATH=$SIMDBENCH_PATH
# e.g., export PYTHONPATH=/home/SimdBench-main
```

### Prerequisites
See `requirements.txt` for python modules.
```bash
$ pip3 install tqdm fire numpy psutil futures tabulate
```
Cmake is also required for the installation of Google Benchmark.

### Enable the Google Benchmark
Building google benchmark using cmake. See [README-googlebenchmark](https://github.com/google/benchmark/blob/main/README.md) for more details.
```bash
$ git submodule update --init --recursive
# or: git clone https://github.com/google/benchmark.git
$ cd benchmark
$ cmake -E make_directory "build"
$ cmake -E chdir "build" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release ../
$ cmake --build "build" --config Release
```
Check the benchmark:
```bash
$ python3 simdbench/check_google_benchmark.py -L benchmark/build/src/ -I benchmark/include/
# return the f-string f"-L{os.path.abspath(src)} -isystem {os.path.abspath(include)}" (compiler flag)
```

## Usage

### Set the C++ compilers for SIMD Intrinsics
Currently supported intrinsic list:
```python
'''
"SSE": "SSE/SSE2 (Streaming SIMD Extensions)",
"AVX": "AVX/AVX2 (Advanced Vector Extensions)",
"SVE": "SVE (Arm C Language Extensions (ACLE) for the Arm Scalable Vector Extension (SVE))",
"Neon": "Neon (Advanced SIMD)",
"RVV": "RVV (RISC-V Vector Extension)"
'''
intrin_list = ["SSE", "AVX", "SVE", "Neon", "RVV"]
```

For the intrinsic under test:
- **Set the compiler path (`cpp_compilers`) in `simdbench/global_var.py`.**:

Check whether your compiler supports the corresponding SIMD intrinsics, e.g.:
```bash
$ python3 simdbench/check_compiler.py -i AVX
```

- **Set the emulator path (`emulators`, if needed) in `simdbench/global_var.py`.**:

Check whether your machine supports the corresponding SIMD instructions, e.g.:
```bash
# in Linux environments
$ cat /proc/cpuinfo | grep Features
```

If the corresponding SIMD instructions are supported by your machine (i.e., emulators are not needed), set emulator `None`, else, use an emulator (e.g., [QEMU](https://www.qemu.org/)). Note: emulators and simulators are not suitable for performance evaluation.


### Getting LLM Completions
The sample format of completions is the same as HumanEval:
```
{"task_id": "XXX", "completion":"XXX"}
```

You can use our samples in `results/model-output`, or use `simdbench/request_llm.py` to generate your samples.
Please note that you need to provide your `api_key="key"` and `base_url="url"` when generating your samples.


### Reproduce the Results
Use our scripts in `scripts` and our model samples in `results/model-output` to reproduce the results.

**Make sure your configuration in `simdbench/global_var.py` is correct before running the scripts.** Emulators are recommended for correctness evaluations, but not for performance evaluations due to performance loss.

```bash
# correctness evaluation: takes about 3-6 hours (256 cpu cores)
nohup python3 ./scripts/correctness_eval.py --correctness_output=[/your/output/path] > pass_at_k.out 2>&1 &
# performance evaluation: takes about 2-4 hours for each intrinsic type
nohup python3 ./scripts/performance_eval.py --intrinsic=[your_intrinsic] --performance_output=[/your/output/path] > intrinsic.out 2>&1 &
```

We also provide our evaluation results in `results/correctness-eval` and `results/performance-eval`.

### Evaluation
Evaluate the correctness of samples like HumanEval, e.g.:
```bash
$ python3 simdbench/evaluate_functional_correctness.py example-task/example_samples.jsonl --intrinsic=AVX --problem_file=example-task/example_problem.jsonl
```

Performance evaluation:
```bash
$ python3 simdbench/evaluate_performance.py example-task/example_samples.jsonl --intrinsic=AVX
```