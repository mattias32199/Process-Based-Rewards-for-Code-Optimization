/*
Given a map, return true if all keys are strings in lower 
case or all keys are strings in upper case, else return false.
The function should return false is the given map is empty.
Examples:
check_map_case_simd({{"a","apple"}, {"b","banana"}}) should return true.
check_map_case_simd({{"a","apple"}, {"A","banana"}, {"B","banana"}}) should return false.
check_map_case_simd({{"a","apple"}, {"8","banana"}, {"a","apple"}}) should return false.
check_map_case_simd({{"Name","John"}, {"Age","36"}, {"City","Houston"}}) should return false.
check_map_case_simd({{"STATE","NC"}, {"ZIP","12345"} }) should return true.

The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
bool check_dict_case_simd(std::map<std::string, std::string> dict){
}
