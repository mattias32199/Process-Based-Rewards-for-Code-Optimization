void double_to_float_saturate(const double* A, float* B, size_t size) {
    for (size_t i = 0; i < size; i++) {
        double val = A[i];
        if (val > FLT_MAX) B[i] = FLT_MAX;
        else if (val < -FLT_MAX) B[i] = -FLT_MAX;
        else B[i] = (float)val;
    }
}