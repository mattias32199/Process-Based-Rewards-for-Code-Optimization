bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(incr_list_simd({}), {}));
    pass = pass && (allclose(incr_list_simd({3, 2, 1}), {4, 3, 2}));
    pass = pass && (allclose(incr_list_simd({5, 2, 5, 2, 3, 3, 9, 0, 123}), {6, 3, 6, 3, 4, 4, 10, 1, 124}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> numbers(length);
        for (size_t j = 0; j < length; j++) {
            numbers[j] = rng.randint( - (1 << 24), (1 << 24));
        }
        
        std::vector<int> result = incr_list(numbers);
        std::vector<int> result_simd = incr_list_simd(numbers);
        
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