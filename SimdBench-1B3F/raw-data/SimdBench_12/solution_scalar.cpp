void mixed_right_shift(const int *src, int *dst, uint8_t shift, size_t length) {
    for (size_t i = 0; i < length; i++) {
        dst[i] = src[i] >> shift; // Arithmetic shift
        dst[i] += (int)((unsigned int)src[i] >> shift); // Logical shift
    }
}
