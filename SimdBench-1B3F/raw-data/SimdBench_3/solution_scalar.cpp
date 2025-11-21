void indexed_access(const float *src, const int *indices, float *dst, size_t length) {
    for (size_t i = 0; i < length; i++) {
        dst[i] = src[indices[i]];
    }
    for (size_t i = 0; i < length; i++) {
        dst[indices[i]] = src[i];
    }
}
