#include <arm_sve.h> 
#include <iostream>
#include <cassert>
#include <cstddef>
#define array_size 15

// -march=armv8-a+sve
void add_float_sve(const float *a, const float *b, float *c, size_t length) {
    size_t i = 0;
    svbool_t pg = svwhilelt_b32(i, length);
    while (svptest_any(svptrue_b32(), pg)){
        svfloat32_t vec_a = svld1_f32(pg, &a[i]);
        svfloat32_t vec_b = svld1_f32(pg, &b[i]);
        svfloat32_t vec_c = svaddv_m(pg, vec_a, vec_b);
        svst1_f32(pg, &c[i], vec_c);
        i += svcntd();
        pg = svwhilelt_b32(i, length);
    }
}

int main(){
    float a[array_size], b[array_size], c[array_size];
    for (int i = 0; i < array_size; i++) {
        a[i] = (float)i;
        b[i] = (float)(i*i);
    }
    add_float_sve(a, b, c, array_size);
    for (int i = 0; i < array_size; i++) {
        std::cout << c[i] << " ";
        assert(c[i] == a[i] + b[i]);
    }
    std::cout << std::endl;
    return 0;
}