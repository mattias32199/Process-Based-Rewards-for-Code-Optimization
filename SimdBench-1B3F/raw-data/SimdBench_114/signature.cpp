/*
Write a function count_nums which takes a vector of integers and returns
the number of elements which has a sum of digits > 0.
If a number is negative, then its first signed digit will be negative:
e.g. -123 has signed digits -1, 2, and 3.
>>> count_nums_simd({}) == 0
>>> count_nums_simd({-1, 11, -11}) == 1
>>> count_nums_simd({1, 1, 2}) == 3

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
int count_nums_simd(std::vector<int> n){
}