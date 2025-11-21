void int_bits_to_float(const uint32_t* A, float* B, size_t size) {
    for (size_t i = 0; i < size; i++) {
        union { uint32_t i; float f; } u;
        u.i = A[i];
        B[i] = u.f;
    }
}