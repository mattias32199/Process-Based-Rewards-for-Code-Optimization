void vector_add_round_nearest(const float* src1, const float* src2, float* dst, size_t length) {
    for (size_t i = 0; i < length; i++) {
        dst[i] = roundf(src1[i] + src2[i]);
    }
}