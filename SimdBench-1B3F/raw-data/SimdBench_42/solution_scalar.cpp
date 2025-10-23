void tensor_bit_clear(const uint64_t* A, const uint64_t* B, uint64_t* C, size_t length) {
    for (size_t idx = 0; idx < length; idx++) {
        C[idx] = A[idx] & (~B[idx]);
    }
}