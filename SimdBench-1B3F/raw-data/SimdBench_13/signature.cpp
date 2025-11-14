/*
The inputs to this function are::
- A pointer to a matrix stored in a non-empty 32-bit integer array (matrix);
- The number of rows in the matrix (rows);
- The number of columns in the matrix (cols).

The input matrix is flattened into a 1D array.
Your goal is to calculate the sum of all elements in the matrix.
The sum should be returned as a 64-bit integer value (the input is guaranteed not to cause an overflow).

The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
int64_t matrix_sum_simd(const int *matrix, size_t rows, size_t cols) {
}