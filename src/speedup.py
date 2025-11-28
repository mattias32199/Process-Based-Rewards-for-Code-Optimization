# /src/verifiers
import re
import subprocess

BENCHMARK_TEMPLATE = """
#include <benchmark/benchmark.h>
#include <immintrin.h>
#include <vector>
#include <random>
#include <cstdint>

class Random {
public:
    std::mt19937 gen{42};  // fixed seed

    template<typename T>
    void initialize_vector_with_random_values(std::vector<T>& vec, bool binary = false) {
        if (binary) {
            // Binary values: 0 or 1
            std::uniform_int_distribution<int> dis(0, 1);
            for (auto& v : vec) {
                v = static_cast<T>(dis(gen));
            }
        } else {
            // Regular values: -100 to 100
            std::uniform_int_distribution<int> dis(-100, 100);
            for (auto& v : vec) {
                v = static_cast<T>(dis(gen));
            }
        }
    }
};

#define Large_Args_1D Arg(1024)->Arg(16384)

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
    simd_code_raw = simd_solution
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
    benchmark_code = benchmark_code.replace('{{SIMD_CODE}}', simd_code)
    benchmark_code = benchmark_code.replace('{{TEST_PERFORMANCE}}', test_performance)

    with open('test.cpp', 'w') as f:
        f.write(benchmark_code)

    # Compile with local benchmark
    compile_result = subprocess.run(
        ['g++', '-std=c++17', '-O3', '-mavx2',
            '-I/content/benchmark/include',
            'test.cpp',
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
        benchmark_result = subprocess.run(['./test'], capture_output=True, text=True)

        if benchmark_result.returncode != 0:
            success = False
            outcome = 'runtime_error'
            feedback = benchmark_result.stderr,
            speedups = None
            avg_speedup = None

        else:
            scalar_times = {}
            simd_times = {}

            pattern = r'(\w+)/(\d+)\s+(\d+)\s+ns'

            for line in benchmark_result.stdout.split('\n'):
                match = re.search(pattern, line)
                if match:
                    name = match.group(1)
                    size = int(match.group(2))
                    time_ns = float(match.group(3))

                    if name == 'Scalar':
                        scalar_times[size] = time_ns
                    elif name == 'SIMD':
                        simd_times[size] = time_ns

            # Calculate speedups
            speedups = {}
            for size in scalar_times:
                if size in simd_times and simd_times[size] > 0:
                    speedup = scalar_times[size] / simd_times[size]
                    speedups[size] = speedup

            # Calculate average
            avg_speedup = sum(speedups.values()) / len(speedups) if speedups else 0.0

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
