bool humaneval_check() {
    bool pass = true;
    pass = pass && (smallest_change_simd({1,2,3,5,4,7,9,6}) == 4);
    pass = pass && (smallest_change_simd({1, 2, 3, 4, 3, 2, 2}) == 1);
    pass = pass && (smallest_change_simd({1, 4, 2}) == 1);
    pass = pass && (smallest_change_simd({1, 4, 4, 2}) == 1);
    pass = pass && (smallest_change_simd({1, 2, 3, 2, 1}) == 0);
    pass = pass && (smallest_change_simd({3, 1, 1, 3}) == 0);
    pass = pass && (smallest_change_simd({1}) == 0);
    pass = pass && (smallest_change_simd({0, 1}) == 1);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int> numbers(length);
        for(auto& number : numbers) {
            number = rng.randint<int>(-10, 10);
        }
        
        int result = smallest_change(numbers);
        int result_simd = smallest_change_simd(numbers);
        
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