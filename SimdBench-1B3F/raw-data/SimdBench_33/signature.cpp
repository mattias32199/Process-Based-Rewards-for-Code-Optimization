/*
The inputs to this function are::
- A pointer to a non-empty 8-bit unsigned integer tensor (A);
- A pointer to a non-empty 8-bit unsigned integer tensor (B);
- An array of 4 size_t integers representing the dimensions of the tensors (dims).

A and B are flattened into 1D arrays.
Your goal is to count the number of equal elements between the two tensors.
The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
size_t tensor_4d_equal_count_simd(const uint8_t* A, const uint8_t* B, const size_t dims[4]) {
}