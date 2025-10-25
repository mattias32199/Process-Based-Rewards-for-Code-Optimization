#include <iostream>
#include <cassert>
#include <cstddef>
#define array_size 15

void add_float(const float *a, const float *b, float *c, size_t length) {
    for (size_t i = 0; i < length; i++){
        c[i] = a[i] + b[i];
    }
}

int main(){
    float a[array_size], b[array_size], c[array_size];
    for (int i = 0; i < array_size; i++) {
        a[i] = (float)i;
        b[i] = (float)(i*i);
    }
    add_float(a, b, c, array_size);
    for (int i = 0; i < array_size; i++) {
        std::cout << c[i] << " ";
        assert(c[i] == a[i] + b[i]);
    }
    std::cout << std::endl;
    return 0;
}