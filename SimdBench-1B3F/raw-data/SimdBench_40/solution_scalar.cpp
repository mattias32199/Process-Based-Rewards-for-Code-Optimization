void tensor_bitwise_or(const uint32_t* A, const uint32_t* B, uint32_t* C, size_t length) {
    for (size_t idx = 0; idx < length; idx++) {
        C[idx] = A[idx] | B[idx];
    }
}