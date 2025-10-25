void int_to_float_sqrt(const uint32_t* A, float* B, size_t size) {
    for (size_t i = 0; i < size; i++) {
        uint16_t high16 = static_cast<uint16_t>(A[i] >> 16);
        float fval = static_cast<float>(high16);
        B[i] = sqrtf(fval);
    }
}