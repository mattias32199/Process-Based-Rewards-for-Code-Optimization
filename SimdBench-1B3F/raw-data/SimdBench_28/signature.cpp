/*
The inputs to this function are::
- A pointer to a non-empty 32-bit floating-point matrix (mat);
- A pointer to a non-empty 32-bit floating-point vector (vec);
- A pointer to a non-empty 32-bit floating-point destination vector (dst);
- The number of rows in the matrix (m);
- The number of columns in the matrix (n).

The matrix is flattened into a 1D array.
mat has the shape (m, n), and vec has the shape (n).
Your goal is to perform matrix-vector multiplication of mat and vec,
and then store the result in the dst vector in the shape (m).

The requirement is to implement the function with [simd] intrinsics for parallelism.

*/
void matrix_vector_mul_simd(const float* mat, const float* vec, float* dst, size_t m, size_t n) {
}
