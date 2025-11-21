/*
The inputs to this function are::
- A pointer to a non-empty 64-bit floating-point matrix (mat1);
- A pointer to a non-empty 64-bit floating-point matrix (mat2);
- A pointer to a non-empty 64-bit floating-point destination matrix (dst);
- The number of rows in the matrices (m);
- The number of columns in the matrices (n).

mat1, mat2, and dst are 2D matrices flattened into 1D arrays.
Your goal is to perform the Hadamard product (element-wise multiplication) of mat1 and mat2,
and then store the result in the dst matrix.

The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void matrix_hadamard_product_simd(const double* mat1, const double* mat2, double* dst, size_t m, size_t n) {
}