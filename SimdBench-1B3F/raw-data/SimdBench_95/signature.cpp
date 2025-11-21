/*
You are given a non-empty vector of positive integers. Return the greatest integer that is greater than 
zero, and has a frequency greater than or equal to the value of the integer itself. 
The frequency of an integer is the number of times it appears in the vector.
If no such a value exist, return -1.
Examples:
    search_simd({4, 1, 2, 2, 3, 1}) == 2
    search_simd({1, 2, 2, 3, 3, 3, 4, 4, 4}) == 3
    search_simd({5, 5, 4, 4, 4}) == -1

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
int search_simd(std::vector<int> lst){
}
