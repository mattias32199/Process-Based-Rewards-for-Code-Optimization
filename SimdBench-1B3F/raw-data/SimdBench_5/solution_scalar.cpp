void extract_tensor_slice(const uint8_t *tensor, uint8_t *slice, size_t dim1, size_t dim2, size_t dim3, size_t slice_idx) {
// Extract 2D slice from 3D tensor at given index
    for (size_t i = 0; i < dim1; i++) {
        for (size_t j = 0; j < dim2; j++) {
            slice[i * dim2 + j] = tensor[(i * dim2 * dim3) + (j * dim3) + slice_idx];
        }
    }
}