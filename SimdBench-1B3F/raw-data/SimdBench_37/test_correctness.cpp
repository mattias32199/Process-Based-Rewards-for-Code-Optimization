bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        bool wrong_case = rng.random_bool();
        size_t block_size = rng.randint(size_t(0), size_t(8));
        size_t rows, cols;
        if(wrong_case || block_size==0){
            rows = Small_Arg_2D;
            cols = Small_Arg_2D;
        }else{
            size_t times = rng.randint(size_t(1), size_t(8));
            rows = block_size * times;
            cols = block_size * times;
        }
        
        std::vector<int> matrix(rows * cols);
        for (size_t r = 0; r < rows; r++) {
            for (size_t c = 0; c < cols; c++) {
                if(block_size > 0) matrix[r * cols + c] = static_cast<int>((r % block_size) + (c % block_size));
                else matrix[r * cols + c] = rng.randint<int>();
            }
        }

        if (wrong_case) {
            size_t r = rng.randint(size_t(0), rows - 1);
            size_t c = rng.randint(size_t(0), cols - 1);
            matrix[r * cols + c] = rng.randint<int>();
        }

        bool scalar_result = matrix_blocks_equal(matrix.data(), block_size, rows, cols);
        bool simd_result = matrix_blocks_equal_simd(matrix.data(), block_size, rows, cols);
        
        if(scalar_result != simd_result){
            if(ERROR_PRINT) {
                std::cerr << "Correctness check failed!" << std::endl;
            }
            return false;
        }
    }
    return true;
}

int main() {
    bool null_pass = matrix_blocks_equal_simd(NULL, 1, 0, 0) == matrix_blocks_equal(NULL, 1, 0, 0);
    bool pass = correctness_check(ITERATIONS) && null_pass;
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}