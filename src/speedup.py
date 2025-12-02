# /src/speedup.py
import re
import subprocess
from pathlib import Path
import json

BENCHMARK_TEMPLATE = """
#include <benchmark/benchmark.h>

// scalar function
{{SCALAR_CODE}}

// simd function
{{SIMD_CODE}}

// benchmark template from SIMDBENCH
{{TEST_PERFORMANCE}}
"""

def extract_code(simd_code_raw: str) -> str:
    """
    Extracts code from model solution
    """
    # try to extract fenced code
    match = re.search(r'```[\w\+\-]*\s*\n(.*?)```', simd_code_raw, re.DOTALL)
    if match:
        code = match.group(1).strip()
    else:
        # Fallback: remove stray backticks if no match
        code = re.sub(r'```+', '', simd_code_raw).strip()
    return code


def verify_speedup(
    task: dict,
    simd_solution: str,
    benchmark_path = '/content/benchmark/build/src/libbenchmark.a' # works for google colab
):
    scalar_code = task['solution_scalar']
    test_performance = task['test_performance']
    # simd_code = extract_code(simd_code_raw)

    # output variables
    success = False
    outcome = 'compilation_error'
    feedback = None # error msg
    speedups = {}
    avg_speedup = None
    scalar_times = None
    simd_times = None

    benchmark_code = BENCHMARK_TEMPLATE.replace('{{SCALAR_CODE}}', scalar_code)
    benchmark_code = benchmark_code.replace('{{SIMD_CODE}}', simd_solution)
    benchmark_code = benchmark_code.replace('{{TEST_PERFORMANCE}}', test_performance)

    with open('test.cpp', 'w') as f:
        f.write(benchmark_code)

    header_path = Path(__file__).parent / "utils.hpp"

    # Compile with local benchmark
    compile_result = subprocess.run(
        ['g++', '-std=c++17', '-O3', '-mavx2',
            '-I/content/benchmark/include',
            'test.cpp',
            f"-include", str(header_path),
            benchmark_path,
            '-o', 'test',
            '-lpthread'],
        capture_output=True,
        text=True,
        timeout=20
    )

    if compile_result.returncode != 0:
        success = False
        outcome = 'compilation_failed'
        feedback = compile_result.stderr,
        speedups = None
        avg_speedup = None

    else:
        # run benchmark
        benchmark_result = subprocess.run(
                ['./test', '--benchmark_format=json'], # Add this flag
                capture_output=True,
                text=True
            )

        if benchmark_result.returncode != 0:
            success = False
            outcome = 'runtime_error'
            feedback = benchmark_result.stderr,
            speedups = None
            avg_speedup = None

        else:
            bench_data = json.loads(benchmark_result.stdout)

            scalar_times = {}
            simd_times = {}

            # 3. Iterate over the 'benchmarks' list in the JSON
            for item in bench_data['benchmarks']:
                # name comes out like "Scalar/1024"
                name_full = item['name']

                # Split "Scalar/1024" -> ["Scalar", "1024"]
                parts = name_full.split('/')
                func_name = parts[0]
                size = int(parts[1])

                # Google benchmark provides 'real_time' (wall clock) or 'cpu_time'
                time_ns = float(item['cpu_time'])

                if func_name == 'Scalar':
                    scalar_times[size] = time_ns
                elif func_name == 'SIMD':
                    simd_times[size] = time_ns

            # Calculate speedups
            speedups = {}
            for size in scalar_times:
                if size in simd_times and simd_times[size] > 0:
                    speedup = scalar_times[size] / simd_times[size]
                    speedups[size] = speedup

            # Calculate average
            if speedups:
                avg_speedup = sum(speedups.values()) / len(speedups)
            else:
                avg_speedup = 0.0
                feedback = bench_data

            success = True
            outcome = "correct_fast" if avg_speedup > 1 else "correct_slow"
            feedback = None


    return {
        'success': success,
        'speedups': speedups,
        'avg_speedup': avg_speedup,
        'scalar_times': scalar_times,
        'simd_times': simd_times,
        'outcome': outcome,
        'feedback': feedback
    }
