void vector_blend(const uint16_t* src1, const uint16_t* src2, uint32_t mask, uint16_t* dst, size_t length) {
    for (size_t i = 0; i < length; i++) {
        dst[i] = (i & mask) ? src1[i] : src2[i];
    }
}
