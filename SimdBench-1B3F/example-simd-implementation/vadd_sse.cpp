#include <xmmintrin.h>
#include <x86intrin.h>
#include <cassert>
#include <cstddef>
#include <iostream>
#define array_size 15

// -msse -msse2
void add_float_sse(const float *a, const float *b, float *c, size_t length) {
    size_t i = 0;
    // Process 4 floats at a time (because SSE registers are 128-bits wide and can hold 4 floats)
    for (; i + 4 <= length; i += 4) {
        __m128 vec_a = _mm_load_ps(&a[i]);
        __m128 vec_b = _mm_load_ps(&b[i]);
        __m128 vec_c = _mm_add_ps(vec_a, vec_b); // Perform the addition in parallel
        _mm_store_ps(&c[i], vec_c); // Store the result back to array c
    }
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
    add_float_sse(a, b, c, array_size);
    for (int i = 0; i < array_size; i++) {
        std::cout << c[i] << " ";
        assert(c[i] == a[i] + b[i]);
    }
    std::cout << std::endl;
    return 0;
}