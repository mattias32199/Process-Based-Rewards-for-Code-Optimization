
bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        std::vector<int> matrix(rows * cols);
        rng.initialize_vector_with_random_values(matrix);

        int64_t scalar_result = matrix_sum(matrix.data(), rows, cols);
        int64_t simd_result = matrix_sum_simd(matrix.data(), rows, cols);
        
        if(scalar_result != simd_result){
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