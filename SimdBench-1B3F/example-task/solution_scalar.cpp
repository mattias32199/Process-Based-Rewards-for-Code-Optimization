void add_float(const float *a, const float *b, float *c, size_t length){
    for (int i = 0; i < length; i++) {
        c[i] = a[i] + b[i];
    }
}