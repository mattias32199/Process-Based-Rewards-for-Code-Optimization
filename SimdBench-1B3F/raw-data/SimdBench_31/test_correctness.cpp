bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        
        std::vector<int16_t> src(length); rng.initialize_vector_with_random_values(src);
        std::vector<int16_t> dst(length);
        std::vector<int16_t> dst_simd(length);

        uint8_t base = rng.randint<uint8_t>(1, 255);

        nearest_multiple(src.data(), dst.data(), base, length);
        nearest_multiple_simd(src.data(), dst_simd.data(), base, length);
        
        if(!allclose(dst, dst_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
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