void tensor_ternary_compare(const float* A, const float* B, int8_t* out, float epsilon, size_t length) {
    for (size_t idx = 0; idx < length; idx++) {
        float diff = A[idx] - B[idx];
        float eps = abs(epsilon);
        if (diff > eps) out[idx] = 1;
        else if (diff < -eps) out[idx] = -1;
        else out[idx] = 0;
    }
}
