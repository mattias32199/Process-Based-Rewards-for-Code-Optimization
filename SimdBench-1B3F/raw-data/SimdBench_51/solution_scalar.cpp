void int_to_float_scaled(const int32_t* A, int8_t scale, float* B, size_t size) {
    for (size_t i = 0; i < size; i++) {
        B[i] = (float)A[i] * scale;
    }
}