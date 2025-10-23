bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(sort_even_simd({1, 2, 3}), {1, 2, 3}));
    pass = pass && (allclose(sort_even_simd({5, 3, -5, 2, -3, 3, 9, 0, 123, 1, -10}), 
                     {-10, 3, -5, 2, -3, 3, 5, 0, 9, 1, 123}));
    pass = pass && (allclose(sort_even_simd({5, 8, -12, 4, 23, 2, 3, 11, 12, -10}),
                     {-12, 8, 3, 4, 5, 2, 12, 11, 23, -10}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<float> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        
        std::vector<float> result = sort_even(numbers);
        std::vector<float> result_simd = sort_even_simd(numbers);
        
        if(!allclose(result, result_simd)){
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