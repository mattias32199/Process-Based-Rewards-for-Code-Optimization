/*
Out of vector of strings, return the longest one. Return the first one in case of multiple
strings of the same length. Return None in case the input vector is empty.
>>> longest_simd({})

>>> longest_simd({"a", "b", "c"})
"a"
>>> longest_simd({"a", "bb", "ccc"})
"ccc"

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::string longest_simd(const std::vector<std::string> & strings){
}
