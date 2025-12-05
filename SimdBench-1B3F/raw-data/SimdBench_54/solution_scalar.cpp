void int16_to_int32(const int16_t* A, int32_t* B, size_t size) {
    for (size_t i = 0; i < size; i++) {
        B[i] = (int32_t)A[i];
    }
}