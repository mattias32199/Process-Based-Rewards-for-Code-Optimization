void load_modify_store(const int *src, int *dst, int scale, size_t length) {
    for (size_t i = 0; i < length; i++) {
        int tmp1 = (src[i] * scale) >> 3;
        int tmp2 = (src[i] * scale) << 3;
        dst[i] = (src[i] * scale + tmp1 * tmp2) / 7;
    }
}