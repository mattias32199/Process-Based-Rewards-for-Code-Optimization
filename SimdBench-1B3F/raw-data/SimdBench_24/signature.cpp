/*
The inputs to this function are::
- A pointer to a non-empty 64-bit floating-point matrix (mat1);
- A pointer to a non-empty 64-bit floating-point matrix (mat2);
- A pointer to a non-empty 64-bit floating-point destination matrix (dst);
- The number of rows in the first matrix (m);
- The number of columns in the first matrix and rows in the second matrix (n);
- The number of columns in the second matrix (p).

mat1, mat2, and dst are 2D matrices flattened into 1D arrays.
Your goal is to perform matrix multiplication of mat1 and mat2,
and then round the result to the nearest integer.
The result should be stored in the dst matrix.

The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void matrix_mul_round_int_simd(const double* mat1, const double* mat2, double* dst, size_t m, size_t n, size_t p) {
}
