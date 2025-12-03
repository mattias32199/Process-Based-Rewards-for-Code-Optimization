# /src/correctness.py
import subprocess
import tempfile
import re
from pathlib import Path
from src.train_util import parse_error_json

TEMPLATE = """
// Scalar solution
{SCALAR_SOLUTION}

// SIMD solution
{SIMD_SOLUTION}

// Test harness
{TEST_HARNESS}
"""

def verify_simd_correctness(
    task_data: dict,
    simd_solution: str,
    timeout: int = 30,
    iterations: int = 100
) -> dict:
    """
    Verify SIMD solution correctness against scalar baseline using SimdBench harness.
    """

    # Extract intrinsic type for compiler flags
    intrinsic = task_data.get("intrinsic", "AVX")
    arch_flags = get_arch_flags(intrinsic)
    scalar_solution=task_data['solution_scalar']
    test_correctness = task_data['test_correctness']

    prefix_part = TEMPLATE.split('{SIMD_SOLUTION}')[0].format(
        SCALAR_SOLUTION=task_data["solution_scalar"],
        ITERATIONS=iterations
    )
    line_offset = prefix_part.count('\n')

    # Generate complete C++ test file
    cpp_code = generate_correctness_test(
        scalar_solution=scalar_solution,
        simd_solution=simd_solution,
        test_harness=test_correctness,
        iterations=iterations
    )

    # intialize output variables
    compiles = False
    correct = False
    outcome = None
    feedback = None

    # Compile and run
    with tempfile.TemporaryDirectory() as tmpdir:
        tmpdir = Path(tmpdir)
        cpp_file = tmpdir / "test.cpp"
        exe_file = tmpdir / "test.out"

        # Write C++ file
        cpp_file.write_text(cpp_code)
        header_path = Path(__file__).parent / "utils.hpp"
        # Compile (test)
        compile_cmd = [
            "g++", "-std=c++17", "-O3",
            "-fdiagnostics-format=json",
            "-include", str(header_path),
            *arch_flags.split(),
            str(cpp_file), "-o", str(exe_file)
        ]

        try:
            compile_result = subprocess.run(
                compile_cmd,
                capture_output=True,
                text=True,
                timeout=timeout
            )

            if compile_result.returncode != 0:
                outcome = "compilation_error"
                feedback = parse_error_json(compile_result.stderr, str(cpp_file), line_offset)
            else:
                compiles = True

        except subprocess.TimeoutExpired:
            outcome = "compilation_error"
            feedback = "Compilation timed out"
        except Exception as e:
            outcome = "compilation_error"
            feedback = f"Compilation error:\n{str(e)}"

        # Run correctness test
        if compiles:
            try:
                result = subprocess.run(
                    [str(exe_file)],
                    capture_output=True,
                    text=True,
                    timeout=timeout
                )

                if result.returncode != 0:
                    outcome = 'runtime_error'
                    feedback = str(result.stderr)

                else:
                    # Parse JSON output
                    output = result.stdout.strip()
                    match = re.search(r'\{\s*"correctness"\s*:\s*"?(\d)"?\s*\}', output)

                    if not match:
                        print(f"Could not parse output:\n{output}")
                        raise NotImplementedError # something is wrong

                    passed = match.group(1) == "1"
                    if passed:
                        correct = True
                    else:
                        outcome = "correctness_error"
                        feedback = "Your solution did not produce the expected outputs."

            except subprocess.TimeoutExpired:
                outcome = 'runtime_error'
                feedback = "Execution error: (timed out)"
            except Exception as e:
                outcome = 'runtime_error'
                feedback = str(e)
    # if not compiles:
    #     print('DEBUG ########')
    #     print('unfiltered feedback\n')
    #     print('feedback')

    #     print('\n\nfiltered feedback\n')

    return {
        'compiles': compiles,
        'correct': correct,
        'outcome': outcome,
        'feedback': feedback
    }


def generate_correctness_test(
    scalar_solution: str, simd_solution: str, test_harness: str, iterations: int
) -> str:
    """Generate complete C++ file with all necessary components."""
    return TEMPLATE.format(
        ITERATIONS=iterations,
        SCALAR_SOLUTION=scalar_solution,
        SIMD_SOLUTION=simd_solution,
        TEST_HARNESS=test_harness
    )


def get_arch_flags(intrinsic: str) -> str:
    """Get compiler flags for specific SIMD instruction set."""

    flags_map = {
        "SSE": "-msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2",
        "AVX": "-mavx -mavx2 -mfma",
        "Neon": "-march=armv8-a+simd",
        "SVE": "-march=armv8-a+sve",
        "RVV": "-march=rv64gcv"
    }

    return flags_map.get(intrinsic, "-march=native")



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



# Example usage
if __name__ == "__main__":
    # Example task data (from SimdBench)
    task_data = {
        "task_id": "SimdBench_0_AVX",
        "solution_scalar": """void conditional_move(const int64_t *src, int64_t *dst, const bool *mask, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (mask[i]) dst[i] = src[i];
    }
}""",
        "test_correctness": """bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++){
        size_t length = Small_Arg_1D;
        std::vector<int64_t> src(length); rng.initialize_vector_with_random_values(src);
        std::vector<int64_t> dst(length); rng.initialize_vector_with_random_values(dst);
        std::vector<int64_t> dst_simd(dst);
        std::vector<uint8_t> mask(length); rng.initialize_vector_with_random_values(mask, true);

        conditional_move(src.data(), dst.data(), reinterpret_cast<const bool*>(mask.data()), length);
        conditional_move_simd(src.data(), dst_simd.data(), reinterpret_cast<const bool*>(mask.data()), length);

        if(!allclose(dst, dst_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    bool pass = correctness_check(ITERATIONS);
    printf("{ \\"correctness\\": %s }\\n", pass ? "1" : "0");
    return 0;
}""",
        "intrinsic": "AVX"
    }

    # Example SIMD solution
    simd_solution = """void conditional_move_simd(const int64_t *src, int64_t *dst, const bool *mask, size_t length) {
    size_t i = 0;
    for (; i + 4 <= length; i += 4) {
        __m256i src_vec = _mm256_loadu_si256((__m256i*)&src[i]);
        __m256i dst_vec = _mm256_loadu_si256((__m256i*)&dst[i]);
        __m256i mask_vec = _mm256_set_epi64x(mask[i+3] ? -1 : 0, mask[i+2] ? -1 : 0,
                                             mask[i+1] ? -1 : 0, mask[i] ? -1 : 0);
        __m256i result = _mm256_blendv_epi8(dst_vec, src_vec, mask_vec);
        _mm256_storeu_si256((__m256i*)&dst[i], result);
    }
    for (; i < length; i++) {
        if (mask[i]) dst[i] = src[i];
    }
}"""

    # Verify
    results = verify_simd_correctness(task_data, simd_solution)
    print(results)
    # print(f"Verification: {'PASSED' if passed else 'FAILED'}")
    # print(f"Message: {message}")
