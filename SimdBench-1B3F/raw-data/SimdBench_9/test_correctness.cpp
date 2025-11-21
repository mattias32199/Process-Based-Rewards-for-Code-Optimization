
bool correctness_check(int iterations) {
    for (int  i = 0; i < iterations; i++){
        /* one check */
        Random rng;
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        std::vector<size_t> indices(rows);
        for (auto & index : indices) {
            index = rng.randint(size_t(0), rows - 1);
        }
        std::vector<double> src(rows * cols); rng.initialize_vector_with_random_values(src);
        std::vector<double> dst(rows * cols); rng.initialize_vector_with_random_values(dst);
        std::vector<double> dst_simd(dst);

        reorder_matrix_rows(src.data(), dst.data(), rows, cols, indices.data());
        reorder_matrix_rows_simd(src.data(), dst_simd.data(), rows, cols, indices.data());
        
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
