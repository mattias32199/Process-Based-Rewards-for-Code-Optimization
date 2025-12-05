/*
You are given a string s.
Your task is to check if the string is happy or not.
A string is happy if its length is at least 3 and every 3 consecutive letters are distinct
For example:
is_happy_simd("a") => false
is_happy_simd("aa") => false
is_happy_simd("abcd") => true
is_happy_simd("aabb") => false
is_happy_simd("adb") => true
is_happy_simd("xyy") => false

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
bool is_happy_simd(const std::string & s){
}
