/*
Write a function that accepts two vectors of strings and returns the vector that has 
total number of chars in the all strings of the vector less than the other vector.

if the two vectors have the same number of chars, return the first vector.

Examples
total_match_simd({}, {}) ➞ {}
total_match_simd({"hi", "admin"}, {"hI", "Hi"}) ➞ {"hI", "Hi"}
total_match_simd({"hi", "admin"}, {"hi", "hi", "admin", "project"}) ➞ {"hi", "admin"}
total_match_simd({"hi", "admin"}, {"hI", "hi", "hi"}) ➞ {"hI", "hi", "hi"}
total_match_simd({"4"}, {"1", "2", "3", "4", "5"}) ➞ {"4"}

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<std::string> total_match_simd(const std::vector<std::string> & lst1, const std::vector<std::string> & lst2){
}
