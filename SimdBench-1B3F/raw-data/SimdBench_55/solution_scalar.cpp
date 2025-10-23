void conditional_normalize(const float* A, const int32_t* control, float* B, size_t size, float min_val, float max_val) {
    for (size_t i = 0; i < size; i++) {
        if (control[i] > 0) {
            float val = A[i];
            val = (val - min_val) / (max_val - min_val);
            B[i] = val < 0.0f ? 0.0f : (val > 1.0f ? 1.0f : val);
        } else {
            B[i] = A[i];
        }
    }
}