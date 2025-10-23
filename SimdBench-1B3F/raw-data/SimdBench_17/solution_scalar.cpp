void saturating_add(const uint16_t* src1, const uint16_t* src2, uint16_t* dst, size_t length) {
    for (size_t i = 0; i < length; i++) {
        uint16_t sum = src1[i] + src2[i];
        dst[i] = (sum < src1[i]) ? UINT16_MAX : sum;
    }
}
