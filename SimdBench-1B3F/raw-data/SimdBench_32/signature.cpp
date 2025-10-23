/*
The inputs to this function are::
- A pointer to a non-empty 64-bit floating-point matrix (matrix);
- A pointer to a non-empty 64-bit floating-point vector (vector);
- The number of rows in the matrix (rows);
- The number of columns in the matrix (cols).

Your goal is to check if any row in the matrix matches the given vector.
The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
bool matrix_has_row_simd(const double* matrix, const double* vector, size_t rows, size_t cols) {
}