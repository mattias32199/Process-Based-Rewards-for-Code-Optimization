#include <immintrin.h>
#include <x86intrin.h>
#include <cassert>
#include <cstddef>
#include <iostream>
#define array_size 15

// -mavx -mavx2
void add_float_avx(const float *a, const float *b, float *c, size_t length) {
    size_t i;
    // Process elements in chunks of 8 (since AVX2 supports 8 floats per register)
    for (i = 0; i + 7 < length; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);  // Load 8 floats from array 'a'
        __m256 vb = _mm256_loadu_ps(&b[i]);  // Load 8 floats from array 'b'
        __m256 vc = _mm256_add_ps(va, vb); // Add the two registers (perform element-wise addition)
        _mm256_storeu_ps(&c[i], vc); // Store the result back into array 'c'
    }
    // Handle remaining elements if length is not a multiple of 8
    for (; i < length; ++i) {
        c[i] = a[i] + b[i];
    }
}

int main(){
    float a[array_size], b[array_size], c[array_size];
    for (int i = 0; i < array_size; i++) {
        a[i] = (float)i;
        b[i] = (float)(i*i);
    }
    add_float_avx(a, b, c, array_size);
    for (int i = 0; i < array_size; i++) {
        std::cout << c[i] << " ";
        assert(c[i] == a[i] + b[i]);
    }
    std::cout << std::endl;
    return 0;
}