bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(derivative_simd({3, 1, 2, 4, 5}), {1, 4, 12, 20}));
    pass = pass && (allclose(derivative_simd({1, 2, 3}), {2, 6}));
    pass = pass && (allclose(derivative_simd({3, 2, 1}), {2, 2}));
    pass = pass && (allclose(derivative_simd({3, 2, 1, 0, 4}), {2, 2, 0, 16}));
    pass = pass && (allclose(derivative_simd({1}), {}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<float> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        
        std::vector<float> result = derivative(numbers);
        std::vector<float> result_simd = derivative_simd(numbers);
        
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