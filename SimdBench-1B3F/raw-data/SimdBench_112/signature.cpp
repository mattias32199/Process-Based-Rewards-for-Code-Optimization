/*
Implement the function f that takes n as a parameter,
and returns a vector of size n, such that the value of the element at index i is the factorial of i modulo 10,000 if i is even
or the sum of numbers from 1 to i otherwise.
i starts from 1.
the factorial of i is the multiplication of the numbers from 1 to i (1 * 2 * ... * i).
Example:
func_simd(5) == {1, 2, 6, 24, 15}

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<int> func_simd(int n){
}