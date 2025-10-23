bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        uint8_t shift = rng.randint<uint8_t>(0, 31);
        std::vector<int> src(length); 
        rng.initialize_vector_with_random_values(src);
        std::vector<int> dst(length);
        std::vector<int> dst_simd(length);

        mixed_right_shift(src.data(), dst.data(), shift, length);
        mixed_right_shift_simd(src.data(), dst_simd.data(), shift, length);
        
        if(!allclose(dst, dst_simd)){
            if(ERROR_PRINT) {
                std::cerr << "Correctness check failed! Shift: " << shift << std::endl;
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