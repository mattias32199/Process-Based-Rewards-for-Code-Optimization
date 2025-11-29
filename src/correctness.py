# /src/correctness.py
import subprocess
import tempfile
import re
from pathlib import Path

TEMPLATE = """
#include <immintrin.h>
#include <x86intrin.h>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>
#include <algorithm>   // For std::swap
#include <limits>      // For std::numeric_limits
#include <type_traits>
#include <cstring>

// Constants
#define ITERATIONS {ITERATIONS}
#define Small_Arg_1D 128
#define ERROR_PRINT true

// Random number generator utility
class Random {{
private:
    std::mt19937_64 gen;
    std::uniform_int_distribution<int64_t> int_dist;
    std::uniform_real_distribution<double> real_dist;

public:
    Random() : gen(std::random_device{{}}()),
               int_dist(std::numeric_limits<int64_t>::min(),
                       std::numeric_limits<int64_t>::max()),
               real_dist(0.0, 1.0) {{}}

    template<typename T>
        T randint(T min, T max) {{
            if constexpr (std::is_integral_v<T>) {{
                if(min > max) std::swap(min, max);
                std::uniform_int_distribution<T> dis(min, max);
                return dis(gen);
            }} else {{
                if(min > max) std::swap(min, max);
                std::uniform_real_distribution<T> dis(min, max);
                return dis(gen);
            }}
        }}

    template<typename T>
    void initialize_vector_with_random_values(std::vector<T>& vec, bool binary = false) {{
        if constexpr (std::is_integral_v<T>) {{
            if (binary) {{
                for (auto& val : vec) {{
                    val = static_cast<T>(gen() % 2);
                }}
            }} else {{
                for (auto& val : vec) {{
                    val = static_cast<T>(int_dist(gen));
                }}
            }}
        }} else {{
            for (auto& val : vec) {{
                val = static_cast<T>(real_dist(gen));
            }}
        }}
    }}
}};

// Comparison utility
template<typename T>
bool allclose(const std::vector<T>& a, const std::vector<T>& b,
              double rtol = 1e-5, double atol = 1e-8) {{
    if (a.size() != b.size()) return false;

    for (size_t i = 0; i < a.size(); i++) {{
        if constexpr (std::is_floating_point_v<T>) {{
            double diff = std::abs(static_cast<double>(a[i]) - static_cast<double>(b[i]));
            double threshold = atol + rtol * std::abs(static_cast<double>(b[i]));
            if (diff > threshold) {{
                if (ERROR_PRINT) {{
                    std::cerr << "Mismatch at index " << i
                             << ": " << a[i] << " vs " << b[i]
                             << " (diff=" << diff << ")" << std::endl;
                }}
                return false;
            }}
        }} else {{
            if (a[i] != b[i]) {{
                if (ERROR_PRINT) {{
                    std::cerr << "Mismatch at index " << i
                             << ": " << a[i] << " vs " << b[i] << std::endl;
                }}
                return false;
            }}
        }}
    }}
    return true;
}}

template<typename T>
bool allclose(const T& a, const T& b, double rtol = 1e-5, double atol = 1e-8) {{
    // 1. Handle Floating Point logic (fuzzy comparison)
    if constexpr (std::is_floating_point_v<T>) {{
        if (std::isnan(a) && std::isnan(b)) return true; // NaN == NaN here
        if (std::isnan(a) || std::isnan(b)) return false;

        double diff = std::abs(static_cast<double>(a) - static_cast<double>(b));
        double threshold = atol + rtol * std::abs(static_cast<double>(b));
        return diff <= threshold;
    }}
    // 2. Handle Integer/Bool logic (exact comparison)
    else {{
        return a == b;
    }}
}}

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

    # Generate complete C++ test file
    cpp_code = generate_correctness_test(
        scalar_solution=task_data["solution_scalar"],
        simd_solution=simd_solution,
        test_harness=task_data["test_correctness"],
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

        # Compile (test)
        compile_cmd = [
            "g++", "-std=c++17", "-O3",
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
                feedback = str(compile_result.stderr)
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
