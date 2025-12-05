void tensor_bit_op(const uint16_t* A, const uint16_t* B, uint16_t* C, size_t length) {
    double half_idx = double(length) / 2.0;
    for (size_t idx = 0; idx < length; idx++){
        if(idx < half_idx) C[idx] = A[idx] ^ B[idx];
        else if(idx == half_idx) C[idx] = A[idx];
        else if(idx > half_idx) C[idx] = A[idx] & B[idx];
    }
}