bool humaneval_check() {
    bool pass = true;
    pass = pass && (specialFilter_simd({5, -2, 1, -5}) == 0);
    pass = pass && (specialFilter_simd({15, -73, 14, -15}) == 1);
    pass = pass && (specialFilter_simd({33, -2, -3, 45, 21, 109}) == 2);
    pass = pass && (specialFilter_simd({43, -12, 93, 125, 121, 109}) == 4);
    pass = pass && (specialFilter_simd({71, -2, -33, 75, 21, 19}) == 3);
    pass = pass && (specialFilter_simd({1}) == 0);
    pass = pass && (specialFilter_simd({}) == 0);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> nums(length);
        rng.initialize_vector_with_random_values(nums);
        
        int result = specialFilter(nums);
        int result_simd = specialFilter_simd(nums);
        
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