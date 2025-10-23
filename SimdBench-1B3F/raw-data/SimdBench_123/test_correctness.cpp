bool humaneval_check() {
    bool pass = true;
    pass = pass && (prod_signs_simd({1, 2, 2, -4}) == -9);
    pass = pass && (prod_signs_simd({0, 1}) == 0);
    pass = pass && (prod_signs_simd({1, 1, 1, 2, 3, -1, 1}) == -10);
    pass = pass && (prod_signs_simd({}) == -32768);
    pass = pass && (prod_signs_simd({2, 4, 1, 2, -1, -1, 9}) == 20);
    pass = pass && (prod_signs_simd({-1, 1, -1, 1}) == 4);
    pass = pass && (prod_signs_simd({-1, 1, 1, 1}) == -4);
    pass = pass && (prod_signs_simd({-1, 1, 1, 0}) == 0);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> arr(length);
        rng.initialize_vector_with_random_values(arr);
        
        int result = prod_signs(arr);
        int result_simd = prod_signs_simd(arr);
        
        if(result != result_simd){
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