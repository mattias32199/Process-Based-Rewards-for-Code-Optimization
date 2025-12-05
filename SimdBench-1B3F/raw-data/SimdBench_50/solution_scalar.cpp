void float_to_int_saturate(const float* A, int32_t* B, size_t size) {
    for (size_t i = 0; i < size; i++) {
        float val = A[i];
        if (val > INT32_MAX) B[i] = INT32_MAX;
        else if (val < INT32_MIN) B[i] = INT32_MIN;
        else B[i] = (int32_t)val;
    }
}