bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        
        std::vector<double> matrix(rows * cols); rng.initialize_vector_with_random_values(matrix);
        std::vector<double> vector(cols); rng.initialize_vector_with_random_values(vector);

        bool correct_case = rng.random_bool();
        if(correct_case){
            size_t idx = rng.randint( size_t(0), rows-1);
            std::copy(vector.begin(), vector.end(), matrix.begin()+idx*cols);
        }

        bool scalar_result = matrix_has_row(matrix.data(), vector.data(), rows, cols);
        bool simd_result = matrix_has_row_simd(matrix.data(), vector.data(), rows, cols);
        
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