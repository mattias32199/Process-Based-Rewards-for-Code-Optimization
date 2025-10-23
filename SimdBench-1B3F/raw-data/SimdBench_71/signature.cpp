/*
From a supplied vector of numbers (of length at least two) select and return two that are the closest to each
other and return them in order (smaller number, larger number).
>>> find_closest_elements_simd({1.0, 2.0, 3.0, 4.0, 5.0, 2.2})
(2.0, 2.2)
>>> find_closest_elements_simd({1.0, 2.0, 3.0, 4.0, 5.0, 2.0})
(2.0, 2.0)

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<float> find_closest_elements_simd(std::vector<float> numbers){
}
