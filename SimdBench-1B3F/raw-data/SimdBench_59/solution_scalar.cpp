void widening_uint(const uint32_t* src, uint64_t* dst, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        uint64_t val = static_cast<uint64_t>(src[i]);
        val |= 0xFFFFFFFF00000000ULL;
        dst[i] = val;
    }
}