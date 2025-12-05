#include <arm_neon.h>
#include <iostream>
#include <cassert>
#include <cstddef>
#define array_size 15

// -march=armv8-a+simd
void add_float_neon(const float *a, const float *b, float *c, size_t length) {
    size_t i;
    for (i = 0; i < length / 4 * 4; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        float32x4_t vc = vaddq_f32(va, vb);
        vst1q_f32(&c[i], vc);
    }
    for (; i < length; i++) {
        c[i] = a[i] + b[i];
    }
}

int main(){
    float a[array_size], b[array_size], c[array_size];
    for (int i = 0; i < array_size; i++) {
        a[i] = (float)i;
        b[i] = (float)(i*i);
    }
    add_float_neon(a, b, c, array_size);
    for (int i = 0; i < array_size; i++) {
        std::cout << c[i] << " ";
        assert(c[i] == a[i] + b[i]);
    }
    std::cout << std::endl;
    return 0;
}