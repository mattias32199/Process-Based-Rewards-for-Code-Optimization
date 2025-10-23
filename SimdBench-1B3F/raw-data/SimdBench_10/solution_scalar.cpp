void vector_vector_add(const int64_t *src1, const int64_t *src2, int64_t *dst, int64_t scalar, size_t length) {
    for (size_t i = 0; i < length; i++) {
        dst[i] = (src1[i] + src2[i] + scalar) / 2;
    }
}
