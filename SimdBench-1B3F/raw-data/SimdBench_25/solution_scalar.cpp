void matrix_transpose_round_quarter(const float* src, float* dst, size_t rows, size_t cols) {
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            dst[j * rows + i] = roundf(src[i * cols + j] * 4.0f) / 4.0f;
        }
    }
}