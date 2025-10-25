bool humaneval_check() {
    bool pass = true;
    pass = pass && (pairs_sum_to_zero_simd({1, 3, 5, 0}) == false);
    pass = pass && (pairs_sum_to_zero_simd({1, 3, -2, 1}) == false);
    pass = pass && (pairs_sum_to_zero_simd({1, 2, 3, 7}) == false);
    pass = pass && (pairs_sum_to_zero_simd({2, 4, -5, 3, 5, 7}) == true);
    pass = pass && (pairs_sum_to_zero_simd({1}) == false);
    pass = pass && (pairs_sum_to_zero_simd({-3, 9, -1, 3, 2, 30}) == true);
    pass = pass && (pairs_sum_to_zero_simd({-3, 9, -1, 3, 2, 31}) == true);
    pass = pass && (pairs_sum_to_zero_simd({-3, 9, -1, 4, 2, 30}) == false);
    pass = pass && (pairs_sum_to_zero_simd({-3, 9, -1, 4, 2, 31}) == false);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int> numbers(length);
        for (size_t j = 0; j < length; j++) {
            numbers[j] = rng.randint( -(1<<10), 1>>10);
        }
        
        bool result = pairs_sum_to_zero(numbers);
        bool result_simd = pairs_sum_to_zero_simd(numbers);
        
        if(result != result_simd){
            if(ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    bool pass = humaneval_check() && correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}
