bool humaneval_check(){
    bool pass = true;
    pass = pass && (allclose(find_closest_elements_simd({1.0, 2.0, 3.9, 4.0, 5.0, 2.2}) , {3.9, 4.0}));
    pass = pass && (allclose(find_closest_elements_simd({1.0, 2.0, 5.9, 4.0, 5.0}) , {5.0, 5.9} ));
    pass = pass && (allclose(find_closest_elements_simd({1.0, 2.0, 3.0, 4.0, 5.0, 2.2}) ,{2.0, 2.2}));
    pass = pass && (allclose(find_closest_elements_simd({1.0, 2.0, 3.0, 4.0, 5.0, 2.0}) ,{2.0, 2.0}));
    pass = pass && (allclose(find_closest_elements_simd({1.1, 2.2, 3.1, 4.1, 5.1}) , {2.2, 3.1}));
    return true;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(2), size_t(512));
        std::vector<float> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        
        std::vector<float> result = find_closest_elements(numbers);
        std::vector<float> result_simd = find_closest_elements_simd(numbers);
        
        if(result.size() != 2 || result_simd.size() != 2 || 
           result[0] != result_simd[0] || result[1] != result_simd[1]){
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