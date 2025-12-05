void matrix_conditional_abs(const int* A, int* B, size_t rows, size_t cols) {
    size_t length = rows*cols;
    for (size_t idx = 0; idx < length; idx++) {
        int val = A[idx];
        B[idx] = (val % 3 == 0) ? (val < 0 ? -val : val) : val;
    }
}