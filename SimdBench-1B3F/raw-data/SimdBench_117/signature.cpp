/*
Given a vector of strings, where each string consists of only digits, return a vector.
Each element i of the output should be 'the number of odd elements in the
string i of the input." where all the i's should be replaced by the number
of odd digits in the i'th string of the input.

>>> odd_count_simd({"1234567"})
{'the number of odd elements 4n the str4ng 4 of the 4nput."}
>>> odd_count_simd({"3","11111111"})
{'the number of odd elements 1n the str1ng 1 of the 1nput.",
 'the number of odd elements 8n the str8ng 8 of the 8nput."}

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::vector<std::string> odd_count_simd(const std::vector<std::string> & lst){
}
