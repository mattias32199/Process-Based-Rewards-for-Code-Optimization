void population_count(const uint32_t* src, uint32_t* dst, size_t length) {
    for (size_t i = 0; i < length; i++) {
        uint32_t x = src[i];
        x = x - ((x >> 1) & 0x55555555);
        x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
        dst[i] = (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
    }
}