void range_matrix_mul(const double* A, const double* B, double* C, size_t m, size_t n, size_t p) {
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < p; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < n; k++) {
                double a_val = A[i * n + k];
                double b_val = B[k * p + j];
                if (a_val >= -100 && a_val <= 100 && b_val >= -100 && b_val <= 100) {
                    sum += a_val * b_val;
                }
            }
            C[i*p + j] = sum;
        }
    }
}