typedef struct {
    int16_t min_num;
    int16_t max_num;
} MinMaxPair;

MinMaxPair min_max_pair(const int16_t* vec, size_t length) {
    MinMaxPair result;
    if (length == 0 || vec == NULL) {
        result.min_num = 0;
        result.max_num = 0;
        return result;
    }
    int16_t min_val = vec[0];
    int16_t max_val = vec[0];

    for (size_t i = 1; i < length; i++) {
        min_val = std::min(vec[i], min_val);
        max_val = std::max(vec[i], max_val);
    }

    result.min_num = min_val;
    result.max_num = max_val;
    return result;
}