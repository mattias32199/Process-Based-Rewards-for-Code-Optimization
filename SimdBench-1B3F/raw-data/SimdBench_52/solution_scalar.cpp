void float_div_round(const float* A, const float* B, int32_t* C, size_t size) {
    for (size_t i = 0; i < size; i++) {
        float val = roundf(A[i] / B[i]);
        if (val > INT32_MAX) C[i] = INT32_MAX;
        else if (val < INT32_MIN) C[i] = INT32_MIN;
        else C[i] = (int32_t)val;
    }
}