void tensor_three_valued_logic(const int8_t* A, const int8_t* B, int8_t* C, size_t length) {
    for (size_t idx = 0; idx < length; idx++) {
        int a = A[idx], b = B[idx];
        if (a == 0 || b == 0) {
            C[idx] = 0;
        } else if (a > 0 && b > 0) {
            C[idx] = 1;
        } else if (a < 0 && b < 0) {
            C[idx] = -1;
        } else {
            C[idx] = 0;
        }
    }
}