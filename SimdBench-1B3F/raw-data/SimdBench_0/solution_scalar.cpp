void conditional_move(const int64_t *src, int64_t *dst, const bool *mask, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (mask[i]) dst[i] = src[i];
    }
}