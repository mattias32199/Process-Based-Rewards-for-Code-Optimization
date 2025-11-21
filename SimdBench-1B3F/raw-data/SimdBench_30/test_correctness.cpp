bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        
        std::vector<int> matrix(rows * cols); for ( auto & element : matrix) { element = rng.randint(- (1 << 8), 1 << 8); }
        std::vector<uint8_t> directions(rows);
        rng.initialize_vector_with_random_values(directions, true);

        // Sort each row according to its direction
        bool sth_wrong = rng.randint(0, 2)%3==0;
        for (size_t i = 0; i < rows; i++) {
            if(sth_wrong){
                bool out_of_order = rng.randint(0, 10)%10==0;
                if(out_of_order) continue;
                else{
                    if(rng.randint(0, 1)==0) std::sort(matrix.begin() + i*cols, matrix.begin() + (i+1)*cols);
                    else std::sort(matrix.begin() + i*cols, matrix.begin() + (i+1)*cols, std::greater<int>());
                }
            } else if (directions[i]) {
                std::sort(matrix.begin() + i*cols, matrix.begin() + (i+1)*cols);
            } else {
                std::sort(matrix.begin() + i*cols, matrix.begin() + (i+1)*cols, std::greater<int>());
            }
        }

        bool scalar_result = matrix_rows_sorted_verify(matrix.data(), reinterpret_cast<const bool*>(directions.data()), rows, cols);
        bool simd_result = matrix_rows_sorted_verify_simd(matrix.data(), reinterpret_cast<const bool*>(directions.data()), rows, cols);
        
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