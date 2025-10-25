void matrix_mul_round_int(const double* mat1, const double* mat2, double* dst, size_t m, size_t n, size_t p) {
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < p; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < n; k++) {
                sum += mat1[i * n + k] * mat2[k * p + j];
            }
            dst[i * p + j] = round(sum);
        }
    }
}