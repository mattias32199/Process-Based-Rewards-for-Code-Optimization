#!/usr/bin/env python3
"""
Debug script to see stderr from benchmark
"""

import sys
import subprocess
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent / 'src'))

from subgoal_verification import GoogleBenchmarkVerifier

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
benchmark_code = verifier.fill_template(scalar_code, simd_code, verifier.template, verifier.test_sizes)

compile_result = verifier.compile_benchmark(benchmark_code)

if not compile_result['success']:
    print(f"Compilation failed: {compile_result['error']}")
    sys.exit(1)

print("Running benchmark and capturing both stdout and stderr...")
result = subprocess.run(
    [compile_result['binary_path'], '--benchmark_format=console'],
    capture_output=True,
    text=True,
    timeout=10
)

print(f"Return code: {result.returncode}")
print("\n=== STDOUT ===")
print(result.stdout)
print("\n=== STDERR ===")
print(result.stderr)
print("=== END ===")

verifier._cleanup(compile_result['tmpdir'])
