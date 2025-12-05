bool correctness_check(int iterations) {
    Random rng;    
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int32_t> src(length);
        std::vector<float> dst(length);
        std::vector<float> dst_simd(length);
        const int8_t test_scale = rng.randint<int8_t>();
        rng.initialize_vector_with_random_values(src);
        
        int_to_float_scaled(src.data(), test_scale, dst.data(), length);
        int_to_float_scaled_simd(src.data(), test_scale, dst_simd.data(), length);      
        
        if (!allclose(dst, dst_simd)) {
            if (ERROR_PRINT) {
                std::cerr << "Correctness check failed at iteration " << i << std::endl;
                std::cerr << "Expected: " << dst[0] << " Got: " << dst_simd[0] << std::endl;
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