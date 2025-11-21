
bool correctness_check(int iterations) {
    Random rng;
    for (int  i = 0; i < iterations; i++){
        /* one check */
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        std::vector<double> src(rows * cols); rng.initialize_vector_with_random_values(src);
        std::vector<double> dst(rows); rng.initialize_vector_with_random_values(dst);
        std::vector<double> dst_simd(dst);
        size_t stride = rng.randint(size_t(0), cols - 1);

        strided_load_store(src.data(), dst.data(), rows, cols, stride);
        strided_load_store_simd(src.data(), dst_simd.data(), rows, cols, stride);
        
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
