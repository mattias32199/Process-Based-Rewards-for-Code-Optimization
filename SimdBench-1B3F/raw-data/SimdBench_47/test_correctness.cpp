bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        size_t total = rows * cols;
        
        std::vector<int> A(total); rng.initialize_vector_with_random_values(A);
        
        std::vector<int> B_scalar(total);
        std::vector<int> B_simd(total);
        matrix_conditional_abs(A.data(), B_scalar.data(), rows, cols);
        matrix_conditional_abs_simd(A.data(), B_simd.data(), rows, cols);
        
        if(!allclose(B_scalar, B_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed at iteration " << i << std::endl;
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