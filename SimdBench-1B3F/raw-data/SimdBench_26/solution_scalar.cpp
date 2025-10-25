void tensor3d_sub_round_zero(const float* tensor1, const float* tensor2, float* dst, size_t dim1, size_t dim2, size_t dim3) {
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim2; j++) {
            for (size_t k = 0; k < dim3; k++) {
                size_t idx = i * dim2 * dim3 + j * dim3 + k;
                dst[idx] = truncf(tensor1[idx] - tensor2[idx]);
            }
        }
    }
}