/*
The inputs to this function are::
- A pointer to a non-empty 32-bit floating-point matrix (src);
- A pointer to a non-empty 32-bit floating-point destination matrix (dst);
- The number of rows in the matrix (rows);
- The number of columns in the matrix (cols).

Your goal is to transpose the matrix src and round each element to the nearest quarter (0.25).
The result should be stored in the dst matrix.

The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void matrix_transpose_round_quarter_simd(const float* src, float* dst, size_t rows, size_t cols) {
}