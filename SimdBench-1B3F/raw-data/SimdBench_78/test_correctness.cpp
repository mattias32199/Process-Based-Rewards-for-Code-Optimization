bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(sort_third_simd({1, 2, 3}), sort_third({1, 2, 3})));
    pass = pass && (allclose(sort_third_simd({5, 3, -5, 2, -3, 3, 9, 0, 123, 1, -10}), 
                     sort_third({5, 3, -5, 2, -3, 3, 9, 0, 123, 1, -10})));
    pass = pass && (allclose(sort_third_simd({5, 8, -12, 4, 23, 2, 3, 11, 12, -10}),
                     sort_third({5, 8, -12, 4, 23, 2, 3, 11, 12, -10})));
    pass = pass && (allclose(sort_third_simd({5, 6, 3, 4, 8, 9, 2}), {2, 6, 3, 4, 8, 9, 5}));
    pass = pass && (allclose(sort_third_simd({5, 8, 3, 4, 6, 9, 2}), {2, 8, 3, 4, 6, 9, 5}));
    pass = pass && (allclose(sort_third_simd({5, 6, 9, 4, 8, 3, 2}), {2, 6, 9, 4, 8, 3, 5}));
    pass = pass && (allclose(sort_third_simd({5, 6, 3, 4, 8, 9, 2, 1}), {2, 6, 3, 4, 8, 9, 5, 1}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        
        std::vector<int> result = sort_third(numbers);
        std::vector<int> result_simd = sort_third_simd(numbers);
        
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