bool correctness_check(int iterations) {
    Random rng;    
    for (int i = 0; i < iterations; i++) {
        const double tolerance = rng.random<double>();
        size_t length = Small_Arg_1D;
        size_t block_size = Small_Arg_2D;
        std::vector<double> vec(length); for(auto & element: vec) { element = rng.uniform<double>(); }
        
        bool correct_case = rng.random_bool();
        if(correct_case && block_size < length && block_size > 0) { 
            size_t num_blocks = length / block_size;
            for(int block_n=1; block_n < num_blocks; block_n++){
                for(int k=0; k<block_size; ++k){
                    vec[block_n*block_size+k] = vec[ k ];
                }
            } 
        }

        bool scalar_result = vector_block_equal(vec.data(), tolerance, length, block_size);
        bool simd_result = vector_block_equal_simd(vec.data(), tolerance, length, block_size);
        
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
    bool corner_pass = true;
    corner_pass = corner_pass && \
    ( vector_block_equal(NULL, 1.0f, 0, 100) == vector_block_equal_simd(NULL, 1.0f, 0, 100) );
    std::vector<double> tmp(100);
    corner_pass = corner_pass && \
    ( vector_block_equal(tmp.data(), 1.0f, 100, 0) == vector_block_equal_simd(tmp.data(), 1.0f, 100, 0) );      
    bool pass = corner_pass && correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}