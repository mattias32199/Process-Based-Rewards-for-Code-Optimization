void matrix_conditional_bit_rotate(const uint32_t* src, uint32_t* dst, uint8_t rotate_bits, size_t length) {
    rotate_bits %= 32;
    for (size_t idx = 0; idx < length; idx++) {
        uint32_t val = src[idx];
        if (val & 0xf == 0xf) {
            dst[idx] = (val >> rotate_bits) | (val << (32 - rotate_bits));
        } else {
            dst[idx] = val;
        }
    }
}