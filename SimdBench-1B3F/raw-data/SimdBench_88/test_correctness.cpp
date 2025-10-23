bool humaneval_check() {
    bool pass = true;
    pass = pass && (below_threshold_simd({1, 2, 4, 10}, 100));
    pass = pass && (!below_threshold_simd({1, 20, 4, 10}, 5));
    pass = pass && (below_threshold_simd({1, 20, 4, 10}, 21));
    pass = pass && (below_threshold_simd({1, 20, 4, 10}, 22));
    pass = pass && (below_threshold_simd({1, 8, 4, 10}, 11));
    pass = pass && (!below_threshold_simd({1, 8, 4, 10}, 10));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int> numbers(length);
        for (size_t j = 0; j < length; j++) {
            numbers[j] = rng.randint<int>(-100, 100);
        }
        int threshold = rng.randint<int>(-1000, 1000);
        
        bool result = below_threshold(numbers, threshold);
        bool result_simd = below_threshold_simd(numbers, threshold);
        
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
