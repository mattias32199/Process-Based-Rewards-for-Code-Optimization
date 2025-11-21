bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        
        std::vector<double> matrix(rows * cols); for(auto & element: matrix) { element = rng.random<double>(); }
        //rng.initialize_vector_with_random_values(matrix);
        std::vector<double> thresholds(rows); for(auto & element: thresholds) { element = rng.normal_distribution(1.0, 0.75); }
        //rng.initialize_vector_with_random_values(thresholds);

        std::vector<uint8_t> result_scalar(rows);
        std::vector<uint8_t> result_simd(rows);

        matrix_row_norm_compare(matrix.data(), thresholds.data(), rows, cols, reinterpret_cast<bool*>(result_scalar.data()));
        matrix_row_norm_compare_simd(matrix.data(), thresholds.data(), rows, cols, reinterpret_cast<bool*>(result_simd.data()));
        
        if(!allclose(result_scalar, result_simd)){
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