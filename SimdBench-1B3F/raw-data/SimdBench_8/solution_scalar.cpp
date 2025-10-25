void conditional_scale(const double *src, double *dst, size_t rows, size_t cols, double threshold, double scale) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            float val = src[i * cols + j];
            dst[i * cols + j] = (val > threshold) ? val * scale : val;
        }
    }
}
