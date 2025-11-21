bool humaneval_check(){
    bool res = true;
    res = res && (allclose(rolling_max_simd({}),{}));
    res = res && (allclose(rolling_max_simd({1, 2, 3, 4}) , {1, 2, 3, 4}));
    res = res && (allclose(rolling_max_simd({4, 3, 2, 1}),{4, 4, 4, 4}));
    res = res && (allclose(rolling_max_simd({3, 2, 3, 100, 3}) ,{3, 3, 3, 100, 100}));
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;

        std::vector<int> numbers(length); rng.initialize_vector_with_random_values(numbers);       
        std::vector<int> res_scalar = rolling_max(numbers);
        std::vector<int> res_simd = rolling_max_simd(numbers);

        if(!allclose(res_scalar, res_simd)){
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