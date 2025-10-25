/*
This function takes a vector l and returns a vector l' such that
l' is identical to l in the odd indicies, while its values at the even indicies are equal
to the values of the even indicies of l, but sorted.
>>> sort_even_simd({1, 2, 3})
{1, 2, 3}
>>> sort_even_simd({5, 6, 3, 4})
{3, 6, 5, 4}

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<float> sort_even_simd(std::vector<float> l){
}
