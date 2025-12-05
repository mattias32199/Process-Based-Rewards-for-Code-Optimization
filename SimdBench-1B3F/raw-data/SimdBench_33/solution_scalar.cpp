size_t tensor_4d_equal_count(const uint8_t* A, const uint8_t* B, const size_t dims[4]) {
    size_t count = 0;
    size_t total = dims[0] * dims[1] * dims[2] * dims[3];
    for (size_t i = 0; i < total; i++) {
        count += (A[i] == B[i]);
    }
    return count;
}
