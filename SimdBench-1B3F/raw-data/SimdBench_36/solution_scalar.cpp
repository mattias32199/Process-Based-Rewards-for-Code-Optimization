bool matrix_rows_strictly_increasing(const int* matrix, size_t rows, size_t cols) {
    if(matrix == NULL || rows <= 0 || cols <= 0) return false;
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 1; j < cols; ++j) {
            if (matrix[i * cols + j] <= matrix[i * cols + j - 1]) {
                return false;
            }
        }
    }
    return true;
}