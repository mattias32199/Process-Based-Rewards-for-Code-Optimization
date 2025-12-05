/*
You"re given a vector of deposit and withdrawal operations on a bank account that starts with
zero balance. Your task is to detect if at any point the balance of account falls below zero, and
at that point function should return true. Otherwise it should return false.
>>> below_zero_simd({1, 2, 3})
false
>>> below_zero_simd({1, 2, -4, 5})
true

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
bool below_zero_simd(std::vector<int> operations){
}