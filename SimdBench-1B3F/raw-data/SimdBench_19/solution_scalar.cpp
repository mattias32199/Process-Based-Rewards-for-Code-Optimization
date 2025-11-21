void tensor_add_3d(const int64_t* A, const int64_t* B, int64_t* C, size_t dim1, size_t dim2, size_t dim3) {
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim2; j++) {
            for (size_t k = 0; k < dim3; k++) {
                size_t idx = i*dim2*dim3 + j*dim3 + k;
                C[idx] = A[idx] + B[idx];
            }
        }
    }
}