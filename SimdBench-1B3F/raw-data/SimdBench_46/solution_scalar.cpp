void tensor_bit_count(const uint32_t* A, uint8_t* out, size_t dim1, size_t dim2, size_t dim3) {
    size_t length = dim1 * dim2 * dim3;
    for(size_t idx = 0; idx < length; ++idx){
        uint32_t val = A[idx];
        uint8_t count = 0;
        while (val) {
            count += val & 1;
            val >>= 1;
        }
        out[idx] = count;
    }
}