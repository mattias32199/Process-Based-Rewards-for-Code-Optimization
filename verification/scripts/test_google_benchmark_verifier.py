#!/usr/bin/env python3
"""
Test script for GoogleBenchmarkVerifier class.
Tests compilation, execution, and parsing of benchmark results.
"""

import sys
from pathlib import Path

# Add parent directory to path to import the verifier
sys.path.insert(0, str(Path(__file__).parent.parent / 'src'))

from subgoal_verification import GoogleBenchmarkVerifier


def test_basic_verification():
    """Test basic scalar vs SIMD verification with simple array addition."""
    print("=" * 60)
    print("Test 1: Basic Array Addition (Scalar vs SIMD)")
    print("=" * 60)

    # Sample scalar implementation
    scalar_code = """
void scalar_func(const float* a, const float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}
"""

    # Sample SIMD implementation using AVX
    simd_code = """
void simd_func(const float* a, const float* b, float* result, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vr = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(result + i, vr);
    }
    // Handle remainder
    for (; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}
"""

    # Initialize verifier with test sizes
    verifier = GoogleBenchmarkVerifier(
        test_sizes=[1024, 4096, 16384],
        isa='AVX'
    )

    print(f"\nVerifier Configuration:")
    print(f"  Compiler: {verifier.compiler}")
    print(f"  ISA: {verifier.isa}")
    print(f"  Test sizes: {verifier.test_sizes}")
    print(f"  Benchmark include: {verifier.benchmark_include}")
    print(f"  Benchmark lib: {verifier.benchmark_lib}")
    print(f"  Template path: {verifier.template_path}")

    # Run verification
    print("\n" + "-" * 60)
    print("Running verification...")
    print("-" * 60)

    result = verifier.verify(scalar_code, simd_code)

    # Display results
    print(f"\nVerification Result:")
    print(f"  Success: {result['success']}")

    if result['success']:
        print(f"\n  Speedups:")
        for size, speedup in result['speedups'].items():
            print(f"    Size {size:6d}: {speedup:.2f}x")

        print(f"\n  Raw Output:")
        print("  " + "-" * 56)
        for line in result['raw_output'].split('\n'):
            if line.strip():
                print(f"  {line}")
    else:
        print(f"  Stage: {result.get('stage', 'unknown')}")
        print(f"  Error: {result.get('error', 'unknown error')}")

    return result['success']


def test_multiplication():
    """Test array multiplication."""
    print("\n" + "=" * 60)
    print("Test 2: Array Multiplication (Scalar vs SIMD)")
    print("=" * 60)

    scalar_code = """
void scalar_func(const float* a, const float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; i++) {
        result[i] = a[i] * b[i];
    }
}
"""

    simd_code = """
void simd_func(const float* a, const float* b, float* result, size_t n) {
    size_t i = 0;
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(a + i);
        __m256 vb = _mm256_loadu_ps(b + i);
        __m256 vr = _mm256_mul_ps(va, vb);
        _mm256_storeu_ps(result + i, vr);
    }
    for (; i < n; i++) {
        result[i] = a[i] * b[i];
    }
}
"""

    verifier = GoogleBenchmarkVerifier(test_sizes=[1024, 4096])

    print("\nRunning verification...")
    result = verifier.verify(scalar_code, simd_code)

    print(f"\nSuccess: {result['success']}")
    if result['success']:
        print("Speedups:")
        for size, speedup in result['speedups'].items():
            print(f"  Size {size:6d}: {speedup:.2f}x")
    else:
        print(f"Error at {result.get('stage')}: {result.get('error')}")

    return result['success']


def test_compilation_error():
    """Test that compilation errors are properly caught."""
    print("\n" + "=" * 60)
    print("Test 3: Compilation Error Handling")
    print("=" * 60)

    # Intentionally broken code
    scalar_code = """
void scalar_func(const float* a, const float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; i++) {
        result[i] = a[i] + b[i]  // Missing semicolon
    }
}
"""

    simd_code = """
void simd_func(const float* a, const float* b, float* result, size_t n) {
    for (size_t i = 0; i < n; i++) {
        result[i] = a[i] + b[i];
    }
}
"""

    verifier = GoogleBenchmarkVerifier(test_sizes=[1024])

    print("\nRunning verification with broken code...")
    result = verifier.verify(scalar_code, simd_code)

    print(f"\nSuccess: {result['success']}")
    print(f"Stage: {result.get('stage', 'N/A')}")

    if not result['success'] and result.get('stage') == 'compilation':
        print("✓ Compilation error correctly detected")
        return True
    else:
        print("✗ Expected compilation error but got different result")
        return False


def main():
    """Run all tests."""
    print("\n" + "=" * 60)
    print("GoogleBenchmarkVerifier Test Suite")
    print("=" * 60)

    tests = [
        ("Basic Array Addition", test_basic_verification),
        ("Array Multiplication", test_multiplication),
        ("Compilation Error Handling", test_compilation_error),
    ]

    results = []
    for name, test_func in tests:
        try:
            success = test_func()
            results.append((name, success))
        except Exception as e:
            print(f"\n✗ Test '{name}' raised exception: {e}")
            results.append((name, False))

    # Summary
    print("\n" + "=" * 60)
    print("Test Summary")
    print("=" * 60)

    passed = sum(1 for _, success in results if success)
    total = len(results)

    for name, success in results:
        status = "✓ PASS" if success else "✗ FAIL"
        print(f"{status}: {name}")

    print(f"\nTotal: {passed}/{total} tests passed")

    return 0 if passed == total else 1

if __name__ == '__main__':
    sys.exit(main())
