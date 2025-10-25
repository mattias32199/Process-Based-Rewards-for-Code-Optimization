void blocked_matrix_transpose(const int16_t *src, int16_t *dst, size_t rows, size_t cols, size_t block_size) {
    for (size_t i = 0; i < rows; i += block_size) {
        for (size_t j = 0; j < cols; j += block_size) {
            size_t i_end = (i + block_size) < rows ? (i + block_size) : rows;
            size_t j_end = (j + block_size) < cols ? (j + block_size) : cols;
            
            for (size_t ii = i; ii < i_end; ii++) {
                for (size_t jj = j; jj < j_end; jj++) {
                    dst[jj * rows + ii] = src[ii * cols + jj];
                }
            }
        }
    }
}