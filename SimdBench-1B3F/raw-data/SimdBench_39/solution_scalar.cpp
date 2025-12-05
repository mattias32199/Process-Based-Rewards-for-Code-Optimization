void matrix_row_norm_compare(const double* matrix, const double* thresholds, size_t rows, size_t cols, bool* result) {
    for (size_t i = 0; i < rows; i++) {
        double sum_sq = 0.0;
        for (size_t j = 0; j < cols; j++) {
            double val = matrix[i*cols + j];
            sum_sq += val * val;
        }
        result[i] = (sqrt(sum_sq) > thresholds[i]);
    }
}