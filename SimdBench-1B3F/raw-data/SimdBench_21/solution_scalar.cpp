void vector_mul_round_up(const float* src1, const float* src2, float* dst, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (i % 2 == 0 && fabsf(src1[i] - src2[i]) <= 50.0f) {
            dst[i] = ceilf(src1[i] * src2[i]);
        } else {
            dst[i] = -1.0f;
        }
    }
}