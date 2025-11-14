bool vector_block_equal(const double* vec, double tolerance, size_t length, size_t block_size) {
    if (vec == NULL || block_size == 0 || length < block_size) {
        return false;
    }
    size_t num_blocks = length / block_size;
    for (size_t i = 1; i < num_blocks; ++i) {
        for (size_t j = 0; j < block_size; ++j) {
            double diff = fabsf(vec[i * block_size + j] - vec[j]);
            if (diff > tolerance) {
                return false;
            }
        }
    }
    return true;
}