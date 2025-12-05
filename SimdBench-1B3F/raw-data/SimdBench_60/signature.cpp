/*
Input to this function are:
- A pointer to a non-empty 64-bit floating-point source array (vec);
- A pointer to a non-empty 16-bit integer (int16_t) index array (index);
- The common length of the arrays (length).

Your goal is to:
For all legal indices `index[i]` (i.e., within the range [0, length)), sum the elements `vec[index[i]]`, 
and return the result.

The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
double indexed_sum_simd(const double* vec, const int16_t* index, size_t length) {
}