/*
The inputs to this function are::
- A pointer to a non-empty 32-bit unsigned integer tensor (A);
- A pointer to a non-empty 8-bit unsigned integer tensor (out);
- The first dimension of the tensor (dim1);
- The second dimension of the tensor (dim2);
- The third dimension of the tensor (dim3).

A and out are 3D tensors flattened into 1D arrays.
Your goal is to count the number of bits set to 1 in each element of tensor A,
and then store the result in tensor out.
The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void tensor_bit_count_simd(const uint32_t* A, uint8_t* out, size_t dim1, size_t dim2, size_t dim3) {
}