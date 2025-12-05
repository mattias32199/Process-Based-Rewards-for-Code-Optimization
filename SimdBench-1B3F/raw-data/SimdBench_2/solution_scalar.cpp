void strided_load_store(const double *src, double *dst, size_t rows, size_t cols, size_t stride) {
    for (size_t r = 0; r < rows; r++) {
        dst[r] = src[r * cols + stride];
    }
}