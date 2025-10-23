/*
This function takes a vector l and returns a vector l' such that
l' is identical to l in the indicies that are not divisible by three, while its values at the indicies that are divisible by three are equal
to the values of the corresponding indicies of l, but sorted.
>>> sort_third_simd({1, 2, 3})
{1, 2, 3}
>>> sort_third_simd({5, 6, 3, 4, 8, 9, 2})
{2, 6, 3, 4, 8, 9, 5}

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<int> sort_third_simd(std::vector<int> l){
}
