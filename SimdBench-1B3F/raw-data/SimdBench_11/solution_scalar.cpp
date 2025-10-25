int vector_even_min(const int *src, size_t length) {
    if(src == NULL || length <= 0) return -1;
    int min_val = src[0];
    for (size_t i = 1; i < length; i++) {
        if (src[i] < min_val && i % 2 == 0) min_val = src[i];
    }
    return min_val;
}