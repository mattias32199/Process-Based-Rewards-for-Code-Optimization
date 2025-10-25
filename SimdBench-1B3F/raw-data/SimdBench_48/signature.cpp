/*
The inputs to this function are::
- A pointer to a non-empty 32-bit unsigned integer vector (src);
- A pointer to a non-empty 32-bit unsigned integer destination vector (dst);
- A 8-bit integer value of the number of bits to rotate (rotate_bits);
- The number of common length of src and dst.

Your goal is to perform a conditional bitwise rotation on the src vector.
The condition in this task is whether the low 4 bits of current element in src are all 1.
If the condition is true, the element in dst should be the result of 
rotating the corresponding element in src by `rotate_bits` bits toward the low bits;
otherwise, the element in dst should be bitwise negation of the corresponding element in src.

The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void matrix_conditional_bit_rotate_simd(const uint32_t* src, uint32_t* dst, uint8_t rotate_bits, size_t length){
}