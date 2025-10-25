bool matrix_blocks_equal(const int* matrix, size_t block_size, size_t rows, size_t cols) {
    if (matrix == NULL) return false;
    if (block_size <= 0) return false;
    if (rows % block_size != 0 || cols % block_size != 0) return false;
    
    size_t block_rows = rows / block_size;
    size_t block_cols = cols / block_size;

    // Compare all blocks to first block
    for (size_t br = 0; br < block_rows; br++) {
        for (size_t bc = 0; bc < block_cols; bc++) {
            for (size_t r = 0; r < block_size; r++) {
                for (size_t c = 0; c < block_size; c++) {
                    size_t idx1 = r*cols + c; // First block
                    size_t idx2 = (br*block_size + r)*cols + (bc*block_size + c);
                    if (matrix[idx1] != matrix[idx2]) return false;
                }
            }
        }
    }
    return true;
}