/*
You are given a vector of numbers.
You need to return the sum of squared numbers in the given vector,
round each element in the vector to the upper int (ceiling) first.
Examples:
For lst = {1,2,3} the output should be 14
For lst = {1,4,9} the output should be 98
For lst = {1,3,5,7} the output should be 84
For lst = {1.4,4.2,0} the output should be 29
For lst = {-2.4,1,1} the output should be 6

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
uint64_t sum_squares_simd(const std::vector<float> & lst){
}
