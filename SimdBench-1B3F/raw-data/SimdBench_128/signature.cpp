/*
Create a function that returns a vector (a, b), where "a" is
the largest of negative integers, and "b" is the smallest
of positive integers in a vector.
If there is no negative or positive integers, return them as 0.

Examples:
largest_smallest_integers_simd({2, 4, 1, 3, 5, 7}) == {0, 1}
largest_smallest_integers_simd({}) == {0,0}
largest_smallest_integers_simd({0}) == {0,0}

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<int> largest_smallest_integers_simd(const std::vector<int>& lst){
}
