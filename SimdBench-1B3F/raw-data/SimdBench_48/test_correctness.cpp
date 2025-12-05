bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        size_t total = rows * cols;
        std::vector<uint32_t> src(total); rng.initialize_vector_with_random_values(src);
        uint8_t rotate_bits = rng.randint<uint8_t>();
        
        std::vector<uint32_t> dst_scalar(total);
        std::vector<uint32_t> dst_simd(total);

        matrix_conditional_bit_rotate(src.data(), dst_scalar.data(), rotate_bits, total);
        matrix_conditional_bit_rotate_simd(src.data(), dst_simd.data(), rotate_bits, total);
        
        if(!allclose(dst_scalar, dst_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed at iteration " << i << std::endl;
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