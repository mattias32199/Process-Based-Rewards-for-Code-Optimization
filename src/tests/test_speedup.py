import os
import pytest
from verifiers.speedup import verify_speedup, benchmark_template

# Modify paths depending on the environment, and how its installed
BENCHMARK_LIB_PATH = '/content/benchmark/build/src/libbenchmark.a'
BENCHMARK_INCLUDE_PATH = '/content/benchmark/include'

SCALAR_CODE = """
void scalar_func(std::vector<int>& vec) {
    volatile int sum = 0;
    for (int v : vec) sum += v;
}
"""

SIMD_CODE = """
void simd_func(std::vector<int>& vec) {
    // This isn't actually SIMD, but it validates the pipeline works
    volatile int sum = 0;
    for (int v : vec) sum += v;
}
"""

TEST_PERFORMANCE = """
static void Scalar(benchmark::State& state) {
    std::vector<int> vec(state.range(0));
    Random r;
    r.initialize_vector_with_random_values(vec);

    for (auto _ : state) {
        scalar_func(vec);
    }
}
// Register Scalar
BENCHMARK(Scalar)->Arg(1024);

static void SIMD(benchmark::State& state) {
    std::vector<int> vec(state.range(0));
    Random r;
    r.initialize_vector_with_random_values(vec);

    for (auto _ : state) {
        simd_func(vec);
    }
}
// Register SIMD
BENCHMARK(SIMD)->Arg(1024);
BENCHMARK_MAIN();
"""

def check_benchmark_dependencies():
    """
    Check if google benchmark library and headers exist
    """
    lib_exists = os.path.exists(BENCHMARK_LIB_PATH)
    include_exists = os.path.exists(BENCHMARK_INCLUDE_PATH)

    if not lib_exists or not include_exists:
        print("\n[WARNING] Google Benchmark dependencies not found!")
        print(f"  - Library found at {BENCHMARK_LIB_PATH}: {lib_exists}")
        print(f"  - Headers found at {BENCHMARK_INCLUDE_PATH}: {include_exists}")
        return False
    return True

def test_verify_speedup():
    """
    Runs a complete check:
    1. Creates valid C++ scalar and fake SIMD code.
    2. Runs verify_speedup.
    3. Checks if code compiles, runs, and parses output correctly.
    """

    # 1. Check Environment
    if not check_benchmark_dependencies():
        # You might want to skip the test here if dependencies are missing
        pass

    task = {
        'solution_scalar': SCALAR_CODE,
        'test_performance': TEST_PERFORMANCE
    }
    result = {
        'completion': SIMD_CODE
    }

    # 4. Run the function under test
    print("Running verify_speedup...")
    output = verify_speedup(task, result, benchmark_template)

    # 5. Assertions

    # Check if compilation failed
    if not output['success'] and output.get('error') == 'compilation_failed':
        print(f"Compilation Error Log:\n{output.get('error_msg')}")
        pytest.fail("cpp compilation failed.")

    # Check if execution failed
    if not output['success'] and output.get('error') == 'execution_failed':
        print(f"Execution Error Log:\n{output.get('error_msg')}")
        pytest.fail("benchmark binary execution failed.")

    # Validation
    assert output['success'] is True, "Function returned success=False"
    assert 'scalar_times' in output
    assert 'simd_times' in output
    assert 1024 in output['scalar_times'], "Did not parse Scalar results for size 1024"
    assert 1024 in output['simd_times'], "Did not parse SIMD results for size 1024"

    # Since we ran the exact same code for scalar and SIMD, speedup should be roughly 1.0
    # We just check that a number exists
    assert isinstance(output['avg_speedup'], float)

    print(f"\nTest Passed! Average Speedup Reported: {output['avg_speedup']:.2f}x")

if __name__ == "__main__":
    print('TEST 1: Google Benchmark Installed???')
    try:
        check_benchmark_dependencies()
    except Exception as e:
        print(f"Test failed: {e}")

    print('TEST 2: Test verify_speedup()')
    try:
        test_verify_speedup()
    except AssertionError as e:
        print(f"\nTest Failed: {e}")
    except Exception as e:
        print(f"\nAn error occurred: {e}")
