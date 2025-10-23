void tensor_multi_select(const float* A, const float* B, const float* C, const float* D, const bool* cond1, const bool* cond2, float* out, size_t dim1, size_t dim2, size_t dim3) {
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim2; j++) {
            for (size_t k = 0; k < dim3; k++) {
                size_t idx = i * dim2 * dim3 + j * dim3 + k;
                bool c1 = cond1[idx], c2 = cond2[idx];
                if (c1 && c2) out[idx] = A[idx];
                else if (c1 && !c2) out[idx] = B[idx];
                else if (!c1 && c2) out[idx] = C[idx];
                else out[idx] = D[idx];
            }
        }
    }
}