void nearest_multiple(const int16_t* src, int16_t* dst, uint8_t base, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        int16_t value = src[i];
        int16_t mode = ((value % base) + base) % base;
        dst[i] = value - mode;
    }
}