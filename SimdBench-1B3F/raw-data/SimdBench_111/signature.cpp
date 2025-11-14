/*
You are given two positive integers n and m, and your task is to compute the
average of the integers from n through m (including n and m). 
Round the answer to the nearest integer(smaller one) and convert that to binary.
If n is greater than m, return "-1".
Example:
rounded_avg_simd(1, 5) => "11"
rounded_avg_simd(7, 5) => "-1"
rounded_avg_simd(10, 20) => "1111"
rounded_avg_simd(20, 33) => "11010"

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::string rounded_avg_simd(int64_t n, int64_t m){
}