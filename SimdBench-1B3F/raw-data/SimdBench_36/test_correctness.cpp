bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        
        std::vector<int> matrix(rows * cols);
        
        for (size_t r = 0; r < rows; r++) {
            for (size_t c = 0; c < cols; c++) {
                matrix[r * cols + c] = static_cast<int>(r * cols + c);
            }
        }

        bool wrong_case = rng.random_bool();
        if (wrong_case && rows > 0 && cols > 0) {
            size_t r = rng.randint(size_t(0), rows-1);
            size_t c = rng.randint(size_t(0), cols-1);
            matrix[r*cols + c] = rng.randint<int>();
        }

        bool scalar_result = matrix_rows_strictly_increasing(matrix.data(), rows, cols);
        bool simd_result = matrix_rows_strictly_increasing_simd(matrix.data(), rows, cols);
        
        if(scalar_result != simd_result){
            if(ERROR_PRINT) {
                std::cerr << "Correctness check failed!" << std::endl;
                std::cerr << "Scalar result: " << scalar_result 
                          << ", SIMD result: " << simd_result << std::endl;
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