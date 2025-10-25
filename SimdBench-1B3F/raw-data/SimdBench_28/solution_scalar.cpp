void matrix_vector_mul(const float* mat, const float* vec, float* dst, size_t m, size_t n) {
    for (size_t i = 0; i < m; i++) {
        float sum = 0.0f;
        for (size_t j = 0; j < n; j++) {
            sum += mat[i * n + j] * vec[j];
        }
        dst[i] = sum;
    }
}