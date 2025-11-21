/*
Input to this function are:
- A pointer to a non-empty 32-bit integer (int32_t) source array (A);
- An int8_t scale factor (scale);
- A pointer to a non-empty 32-bit floating-point (float) destination array (B);
- The size of the arrays (size).

Your goal is to convert the int32_t values in A to values in float type in B,
and scale them by the given scale factor.

The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void int_to_float_scaled_simd(const int32_t* A, int8_t scale, float* B, size_t size) {
}