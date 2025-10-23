bool humaneval_check(){
    std::vector<double> a={1.0, 2.0, 3.9, 4.0, 5.0, 2.2};
    bool res = true;
    res = res && (has_close_elements_simd(a, 0.3)==true);
    res = res && (has_close_elements_simd(a, 0.05) == false);

    res = res && (has_close_elements_simd({1.0, 2.0, 5.9, 4.0, 5.0}, 0.95) == true);
    res = res && (has_close_elements_simd({1.0, 2.0, 5.9, 4.0, 5.0}, 0.8) ==false);
    res = res && (has_close_elements_simd({1.0, 2.0, 3.0, 4.0, 5.0}, 2.0) == true);
    res = res && (has_close_elements_simd({1.1, 2.2, 3.1, 4.1, 5.1}, 1.0) == true);
    res = res && (has_close_elements_simd({1.1, 2.2, 3.1, 4.1, 5.1}, 0.5) == false);
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;

        std::vector<double> numbers(length); rng.initialize_vector_with_random_values(numbers);
        double threshold = rng.normal_distribution<double>();
        
        bool res_scalar = has_close_elements(numbers, threshold);
        bool res_simd = has_close_elements_simd(numbers, threshold);

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