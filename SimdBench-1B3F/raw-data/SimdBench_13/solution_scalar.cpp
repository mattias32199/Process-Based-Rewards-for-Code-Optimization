int64_t matrix_sum(const int *matrix, size_t rows, size_t cols) {
    int64_t sum = 0;
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            sum += matrix[i * cols + j];
        }
    }
    return sum;
}