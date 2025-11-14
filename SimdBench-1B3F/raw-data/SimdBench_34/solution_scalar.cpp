void axm_abs(size_t length, const int64_t a, const int64_t *x, int64_t *y) {
    for (size_t i = 0; i < length; ++i) {
        y[i] = a * x[i] - llabs(y[i]);
    }
}