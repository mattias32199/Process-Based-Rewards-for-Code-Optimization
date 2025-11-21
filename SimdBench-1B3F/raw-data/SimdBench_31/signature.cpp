/* 
The inputs to this function are::
- A pointer to a non-empty 16-bit integer source array (src);
- A pointer to a non-empty 16-bit integer destination array (dst);
- A 8-bit unsigned integer (base) that is greater than 0;
- The common length of src and dst (length).

Your goal is, for each element in src, to store in dst the nearest integer that 
1) is a multiple of base, and 
2) has a value no more than the value of the element in src.
No overflow in the input.
The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void nearest_multiple_simd(const int16_t* src, int16_t* dst, uint8_t base, size_t length) {
}