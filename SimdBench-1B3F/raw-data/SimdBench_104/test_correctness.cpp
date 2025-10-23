bool humaneval_check() {
    bool pass = true;
    pass = pass && (add_simd({4, 88}) == 88);
    pass = pass && (add_simd({4, 5, 6, 7, 2, 122}) == 122);
    pass = pass && (add_simd({4, 0, 6, 7}) == 0);
    pass = pass && (add_simd({4, 4, 6, 8}) == 12);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        
        int result = add(numbers);
        int result_simd = add_simd(numbers);
        
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