void reorder_matrix_rows(const double *src, double *dst, size_t rows, size_t cols, const size_t *indices) {
    for (size_t i = 0; i < rows; i++) {
        size_t src_row = indices[i];
        for (size_t j = 0; j < cols; j++) {
            dst[i * cols + j] = src[src_row * cols + j];
        }
    }
}