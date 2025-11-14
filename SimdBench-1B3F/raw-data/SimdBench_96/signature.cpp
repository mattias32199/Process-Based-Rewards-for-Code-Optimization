/*
Write a function that returns true if the non-empty object q will fly, and false otherwise.
The object q will fly if it's balanced (it is a palindromic vector) and the sum of its elements is less than or equal the maximum possible weight w.

Example:
will_it_fly_simd({1, 2}, 5) ➞ false 
// 1+2 is less than the maximum possible weight, but it's unbalanced.

will_it_fly_simd({3, 2, 3}, 1) ➞ false
// it's balanced, but 3+2+3 is more than the maximum possible weight.

will_it_fly_simd({3, 2, 3}, 9) ➞ true
// 3+2+3 is less than the maximum possible weight, and it's balanced.

will_it_fly_simd({3}, 5) ➞ true
// 3 is less than the maximum possible weight, and it's balanced.

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
bool will_it_fly_simd(std::vector<int> q,int w){
}
