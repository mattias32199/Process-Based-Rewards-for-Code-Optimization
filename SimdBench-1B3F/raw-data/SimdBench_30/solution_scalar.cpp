bool matrix_rows_sorted_verify(const int* matrix, const bool* directions, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; i++) {
        bool ascending = directions[i];
        for (size_t j = 1; j < cols; j++) {
            int curr = matrix[i*cols + j];
            int prev = matrix[i*cols + (j-1)];
            if ((ascending && curr < prev) || (!ascending && curr > prev)) {
                return false;
            }
        }
    }
    return true;
}