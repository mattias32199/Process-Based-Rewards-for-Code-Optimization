bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<uint32_t> src(length); 
        rng.initialize_vector_with_random_values(src);
        std::vector<uint32_t> dst(length);
        std::vector<uint32_t> dst_simd(length);

        population_count(src.data(), dst.data(), length);
        population_count_simd(src.data(), dst_simd.data(), length);
        
        if(!allclose(dst, dst_simd)){
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