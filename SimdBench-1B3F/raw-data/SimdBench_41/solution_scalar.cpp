int64_t squarediff(const int8_t * A, const int8_t * B, size_t length) {
    int64_t sum1 = 0, sum2 = 0;
    for (int i = 0; i < length; i++) {
        sum1 += (int64_t)A[i] * A[i] + (int64_t)B[i] * B[i];
        sum2 += (int64_t)A[i] * B[i];
    }
    return sum1 - sum2;
}