void matrix_hadamard_product(const double* mat1, const double* mat2, double* dst, size_t m, size_t n) {
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            dst[i * n + j] = mat1[i * n + j] * mat2[i * n + j];
        }
    }
}