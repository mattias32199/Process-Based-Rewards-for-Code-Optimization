/*
You are given a vector arr of integers and you need to return
sum of magnitudes of integers multiplied by product of all signs
of each number in the vector, represented by 1, -1 or 0.
Note: return -32768 for empty arr.

Example:
>>> prod_signs_simd({1, 2, 2, -4}) == -9
>>> prod_signs_simd({0, 1}) == 0
>>> prod_signs_simd({}) == -32768

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
int prod_signs_simd(const std::vector<int> & arr){
}