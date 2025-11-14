bool humaneval_check() {
    bool pass = true;
    pass = pass && (can_arrange_simd({1,2,4,3,5}) == 3);
    pass = pass && (can_arrange_simd({1,2,4,5}) == -1);
    pass = pass && (can_arrange_simd({1,4,2,5,6,7,8,9,10}) == 2);
    pass = pass && (can_arrange_simd({4,8,5,7,3}) == 4);
    pass = pass && (can_arrange_simd({}) == -1);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> arr(length);
        rng.initialize_vector_with_random_values(arr);
        
        int result = can_arrange(arr);
        int result_simd = can_arrange_simd(arr);
        
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