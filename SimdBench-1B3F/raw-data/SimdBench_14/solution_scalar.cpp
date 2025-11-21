size_t argmax(const int8_t *src ,size_t length) {
    if(src == NULL) return size_t(0);
    size_t index = 0;
    for (size_t i = 1; i < length; i++) {
        if (src[i] > src[index]) index = i;
    }
    return index;
}