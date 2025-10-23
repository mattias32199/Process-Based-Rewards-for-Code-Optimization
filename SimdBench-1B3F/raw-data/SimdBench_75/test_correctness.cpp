bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(get_positive_simd({-1, -2, 4, 5, 6}), { 4, 5, 6}));
    pass = pass && (allclose(get_positive_simd({5, 3, -5, 2, 3, 3, 9, 0, 123, 1, -10}), {5, 3, 2, 3, 3, 9, 123, 1}));
    pass = pass && (allclose(get_positive_simd({-1, -2}), {}));
    pass = pass && (allclose(get_positive_simd({}), {}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<float> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        
        std::vector<float> result = get_positive(numbers);
        std::vector<float> result_simd = get_positive_simd(numbers);
        
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