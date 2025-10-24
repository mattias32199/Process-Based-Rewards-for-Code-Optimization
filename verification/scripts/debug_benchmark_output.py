#!/usr/bin/env python3
"""
Debug script to see actual benchmark output
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / 'src'))

from subgoal_verification import GoogleBenchmarkVerifier

# Simple scalar and SIMD code
scalar_code = """
void scalar_func(const float* a, const float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}
"""

simd_code = """
void simd_func(const float* a, const float* b, float* result, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(result + i, vr);
    }
    for (; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}
"""

verifier = GoogleBenchmarkVerifier(test_sizes=[1024], isa='AVX')

# Get filled template
benchmark_code = verifier.fill_template(scalar_code, simd_code, verifier.template, verifier.test_sizes)

# Compile
print("Compiling...")
compile_result = verifier.compile_benchmark(benchmark_code)

if not compile_result['success']:
    print(f"Compilation failed: {compile_result['error']}")
    sys.exit(1)

print("✓ Compilation successful")

# Run
print("\nRunning benchmark...")
run_result = verifier.run_benchmark(compile_result['binary_path'])

print(f"\nReturn code: {run_result.get('success')}")
print("\n=== STDOUT ===")
print(run_result.get('output', run_result.get('error', 'No output')))
print("=== END ===")

# Cleanup
verifier._cleanup(compile_result['tmpdir'])
