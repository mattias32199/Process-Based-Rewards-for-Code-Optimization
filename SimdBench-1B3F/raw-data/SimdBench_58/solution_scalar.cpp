void conditional_diff(const int32_t* A, const bool* cond, float* diff, size_t size) {
    diff[0] = 0.0f;
    for (size_t i = 1; i < size; i++) {
        if (cond[i]) {
            diff[i] = (float)(A[i] - A[i-1]);
        } else {
            diff[i] = 0.0f;
        }
    }
}