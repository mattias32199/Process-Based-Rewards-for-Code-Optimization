bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(pluck_simd({4,2,3}), {2, 1}));
    pass = pass && (allclose(pluck_simd({1,2,3}), {2, 1}));
    pass = pass && (allclose(pluck_simd({}), {}));
    pass = pass && (allclose(pluck_simd({5, 0, 3, 0, 4, 2}), {0, 1}));
    pass = pass && (allclose(pluck_simd({1, 2, 3, 0, 5, 3}), {0, 3}));
    pass = pass && (allclose(pluck_simd({5, 4, 8, 4 ,8}), {4, 1}));
    pass = pass && (allclose(pluck_simd({7, 6, 7, 1}), {6, 1}));
    pass = pass && (allclose(pluck_simd({7, 9, 7, 1}), {}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        
        std::vector<int> result = pluck(numbers);
        std::vector<int> result_simd = pluck_simd(numbers);
        
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