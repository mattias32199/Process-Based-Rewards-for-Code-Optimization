/*
Given a vector of numbers, return the sum of squares of the numbers
in the vector that are odd. Ignore numbers that are negative or not integers.

double_the_difference_simd({1, 3, 2, 0}) == 1 + 9 + 0 + 0 = 10
double_the_difference_simd({-1, -2, 0}) == 0
double_the_difference_simd({9, -2}) == 81
double_the_difference_simd({0}) == 0  

If the input vector is empty, return 0.

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
uint64_t double_the_difference_simd(const std::vector<float> & lst){
}
