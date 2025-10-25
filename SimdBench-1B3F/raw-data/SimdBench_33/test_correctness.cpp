bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t dims[4] = {Small_Arg_4D, Small_Arg_4D, Small_Arg_4D, Small_Arg_4D};
        size_t total = dims[0] * dims[1] * dims[2] * dims[3];
        
        std::vector<uint8_t> A(total); rng.initialize_vector_with_random_values(A);
        std::vector<uint8_t> B(total); rng.initialize_vector_with_random_values(B);

        size_t n = rng.randint(size_t(0), total);
        while(n-- && total > 0){
            size_t idx = rng.randint(size_t(0), total-1);
            B[idx] = A[idx];
        }

        size_t scalar_result = tensor_4d_equal_count(A.data(), B.data(), dims);
        size_t simd_result = tensor_4d_equal_count_simd(A.data(), B.data(), dims);
        
        if(scalar_result != simd_result){
            if(ERROR_PRINT) {
                std::cerr << "Correctness check failed!" << std::endl;
                std::cerr << "Scalar count: " << scalar_result 
                          << ", SIMD count: " << simd_result << std::endl;
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