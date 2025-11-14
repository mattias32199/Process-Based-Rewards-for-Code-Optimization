void matrix_scalar_mul(const double* mat, double scalar, double* dst, size_t m, size_t n) {
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            dst[i * n + j] = mat[i * n + j] * scalar;
        }
    }
}