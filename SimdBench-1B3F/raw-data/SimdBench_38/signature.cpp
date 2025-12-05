/*
The inputs to this function are::
- A pointer to a 64-bit floating-point vector (vec);
- A 64-bit floating-point tolerance;
- The length of the given vector (length);
- The size of each block (block_size).

Your goal is to check whether all blocks (excluding tail elements that cannot form a complete block)
of the vector are element-wise equal to the first block, within the specified tolerance.
Return false is the input is illegal or no complete blocks in block_size.
The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
bool vector_block_equal_simd(const double* vec, double tolerance, size_t length, size_t block_size) {
}