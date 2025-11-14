bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int8_t> src(length);
        rng.initialize_vector_with_random_values(src);

        size_t scalar_result = argmax(src.data(), length);
        size_t simd_result = argmax_simd(src.data(), length);
        
        if(scalar_result != simd_result){
            if(ERROR_PRINT) {
                std::cerr << "Correctness check failed!" << std::endl;
                std::cerr << "Scalar result index: " << scalar_result 
                          << " (value: " << src[scalar_result] << ")" << std::endl;
                std::cerr << "SIMD result index: " << simd_result
                          << " (value: " << src[simd_result] << ")" << std::endl;
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