/*
The inputs to this function are::
- The common length of the given vectors;
- A 64-bit integer `a`;
- Two pointers to 64-bit integer arrays.

Your goal is to do the element-wise vector operation involving:
1. scalar-vector multiplication a and x;
2. absolute value of vector: y;
3. element-wise subtraction.
Store the results to `y`.

No overflow in the input.
The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void axm_abs_simd(size_t length, const int64_t a, const int64_t *x, int64_t *y) {
}