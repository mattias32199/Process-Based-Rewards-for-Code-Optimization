bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(rescale_to_unit_simd({2.0, 49.9}), {0.0, 1.0}));
    pass = pass && (allclose(rescale_to_unit_simd({100.0, 49.9}), {1.0, 0.0}));
    pass = pass && (allclose(rescale_to_unit_simd({1.0, 2.0, 3.0, 4.0, 5.0}), {0.0, 0.25, 0.5, 0.75, 1.0}));
    pass = pass && (allclose(rescale_to_unit_simd({2.0, 1.0, 5.0, 3.0, 4.0}), {0.25, 0.0, 1.0, 0.5, 0.75}));
    pass = pass && (allclose(rescale_to_unit_simd({12.0, 11.0, 15.0, 13.0, 14.0}), {0.25, 0.0, 1.0, 0.5, 0.75}));
    return pass;
}

bool correctness_check(int iterations) {

    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(2), size_t(128));
        std::vector<float> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        
        std::vector<float> result = rescale_to_unit(numbers);
        std::vector<float> result_simd = rescale_to_unit_simd(numbers);
        
        
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