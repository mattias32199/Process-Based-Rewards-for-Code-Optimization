bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(0), size_t(1024));
        std::vector<int> src(length); 
        rng.initialize_vector_with_random_values(src);

        int scalar_result = vector_even_min(src.data(), length);
        int simd_result = vector_even_min_simd(src.data(), length);
        
        if(scalar_result != simd_result){
            if(ERROR_PRINT) {
                std::cerr << "Correctness check failed!" << std::endl;
            }
            return false;
        }
    }
    return true;
}

int main() {
    bool pass = correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}