/*
The inputs to this function are::
- A pointer to a non-empty 64-bit floating-point source matrix (mat);
- A scalar value in 64-bit floating-point (scalar);
- A pointer to a non-empty 64-bit floating-point destination matrix (dst);
- The number of rows in the matrix (m);
- The number of columns in the matrix (n).

mat and dst are 2D matrices flattened into 1D arrays.
Your goal is to calculate the result of the element-wise multiplication of mat and scalar,
and then store the result in the dst matrix.

The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void matrix_scalar_mul_simd(const double* mat, double scalar, double* dst, size_t m, size_t n) {
}