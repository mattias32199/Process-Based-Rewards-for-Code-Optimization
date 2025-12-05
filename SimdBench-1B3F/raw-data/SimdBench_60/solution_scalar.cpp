double indexed_sum(const double* vec, const int16_t* index, size_t length) {
    double sum = 0.0;
    for (size_t i = 0; i < length; ++i) {
        if (index[i] < length && index[i] >= 0) {
            sum += vec[index[i]];
        }
    }
    return sum;
}
