bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        
        std::vector<float> src(rows * cols); 
        rng.initialize_vector_with_random_values(src);
        std::vector<float> dst(rows * cols);
        std::vector<float> dst_simd(rows * cols);

        matrix_transpose_round_quarter(src.data(), dst.data(), rows, cols);
        matrix_transpose_round_quarter_simd(src.data(), dst_simd.data(), rows, cols);
        
        if(!allclose(dst, dst_simd, 1e-6f, 1e-6f)){
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