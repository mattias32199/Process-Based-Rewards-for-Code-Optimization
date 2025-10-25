/*
Given a non-empty vector of integers arr and an integer k, return
the sum of the elements with at most two digits from the first k elements of arr.

Example:
    Input: arr = {111,21,3,4000,5,6,7,8,9}, k = 4
    Output: 24 # sum of 21 + 3

Constraints:
    1. 1 <= len(arr) <= 2^24
    2. 1 <= k <= len(arr)

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
int add_elements_simd(std::vector<int> arr, int k){
}
