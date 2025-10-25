double matrix_average(const double* mat, size_t m, size_t n) {
    double sum = 0.0;
    for (size_t j = 0; j < n; j++) {
        for (size_t i = 0; i < m; i++) {
            sum += mat[i * n + j];
        }
    }
    return sum / double(m * n);
}