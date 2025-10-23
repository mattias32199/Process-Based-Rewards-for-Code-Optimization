/*
Return only positive numbers in the vector.
>>> get_positive_simd({-1, 2, -4, 5, 6})
{2, 5, 6}
>>> get_positive_simd({5, 3, -5, 2, -3, 3, 9, 0, 123, 1, -10})
{5, 3, 2, 3, 9, 123, 1}

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<float> get_positive_simd(const std::vector<float> & l){
}