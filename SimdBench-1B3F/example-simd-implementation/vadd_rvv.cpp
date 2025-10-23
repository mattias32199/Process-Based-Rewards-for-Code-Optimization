#include <riscv_vector.h>
#include <iostream>
#include <cassert>
#include <cstddef>
#define array_size 15

// -march=rv64gcv -mabi=lp64d -static
void add_float_rvv(const float *a, const float *b, float *c, size_t length) {
    size_t avl = length;
    while (avl > 0) {
        size_t vl = __riscv_vsetvl_e32m2(avl);
        vfloat32m2_t va = __riscv_vle32_v_f32m2(a, vl);
        vfloat32m2_t vb = __riscv_vle32_v_f32m2(b, vl);
        vfloat32m2_t vc = __riscv_vfadd_vv_f32m2(va, vb, vl);
        __riscv_vse32_v_f32m2(c, vc, vl);
        a += vl; b += vl; c += vl;
        avl -= vl;
    }
}

int main(){
    float a[array_size], b[array_size], c[array_size];
    for (int i = 0; i < array_size; i++) {
        a[i] = (float)i;
        b[i] = (float)(i*i);
    }
    add_float_rvv(a, b, c, array_size);
    for (int i = 0; i < array_size; i++) {
        std::cout << c[i] << " ";
        assert(c[i] == a[i] + b[i]);
    }
    std::cout << std::endl;
    return 0;
}