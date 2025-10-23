/*
xs represent coefficients of a polynomial.
xs{0} + xs{1} * x + xs{2} * x^2 + ....
 Return derivative of this polynomial in the same form.
>>> derivative_simd({3, 1, 2, 4, 5})
{1, 4, 12, 20}
>>> derivative_simd({1, 2, 3})
{2, 6}

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<float> derivative_simd(std::vector<float> xs){
}