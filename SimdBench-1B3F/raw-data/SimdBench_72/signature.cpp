/*
Given vector of numbers (of at least two elements), apply a linear transform to that vector,
such that the smallest number will become 0 and the largest will become 1
>>> rescale_to_unit_simd({1.0, 2.0, 3.0, 4.0, 5.0})
{0.0, 0.25, 0.5, 0.75, 1.0}

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<float> rescale_to_unit_simd(std::vector<float> numbers){ 
}
