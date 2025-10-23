float diagonal_sum_3d(const float *array, size_t dim) {
    float sum = 0.0f;
    for (size_t i = 0; i < dim; i++) {
        sum += array[i * dim * dim + i * dim + i]; // Accessing the diagonal element
    }
    return sum;
}