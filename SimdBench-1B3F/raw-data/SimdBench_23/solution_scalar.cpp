void matrix_normalize_rows(float* mat, size_t m, size_t n) {
    for (size_t i = 0; i < m; i++) {
        float norm = 0.0f;
        for (size_t j = 0; j < n; j++) {
            norm += mat[i * n + j] * mat[i * n + j];
        }
        norm = sqrtf(norm);
        if (norm > 1e-8f) {
            for (size_t j = 0; j < n; j++) {
                mat[i * n + j] /= norm;
            }
        }
    }
}