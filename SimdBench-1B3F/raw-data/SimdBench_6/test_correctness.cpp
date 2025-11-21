bool correctness_check(int iterations) {
    Random rng;
    for (int  i = 0; i < iterations; i++){
        /* one check */
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        size_t block_size = rng.randint(size_t(1), std::min(rows, cols));
        std::vector<int16_t> src(rows * cols); rng.initialize_vector_with_random_values(src);
        std::vector<int16_t> dst(rows * cols); rng.initialize_vector_with_random_values(dst);
        std::vector<int16_t> dst_simd(dst);

        blocked_matrix_transpose(src.data(), dst.data(), rows, cols, block_size);
        blocked_matrix_transpose_simd(src.data(), dst_simd.data(), rows, cols, block_size);
        
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
