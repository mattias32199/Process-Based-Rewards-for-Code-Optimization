/*
Given a vector of numbers, return whether or not they are sorted
in ascending order. If vector has more than 1 duplicate of the same
number, return false. Assume no negative numbers and only integers.

Examples
is_sorted_simd({5}) ➞ true
is_sorted_simd({1, 2, 3, 4, 5}) ➞ true
is_sorted_simd({1, 3, 2, 4, 5}) ➞ false
is_sorted_simd({1, 2, 3, 4, 5, 6}) ➞ true
is_sorted_simd({1, 2, 3, 4, 5, 6, 7}) ➞ true
is_sorted_simd({1, 3, 2, 4, 5, 6, 7}) ➞ false
is_sorted_simd({1, 2, 2, 3, 3, 4}) ➞ true
is_sorted_simd({1, 2, 2, 2, 3, 4}) ➞ false

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
bool is_sorted_simd(const std::vector<int> & lst){
}
