bool matrix_has_row(const double* matrix, const double* vector, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; i++) {
        bool match = true;
        for (size_t j = 0; j < cols; j++) {
            if (matrix[i * cols + j] != vector[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}