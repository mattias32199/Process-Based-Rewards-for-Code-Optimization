bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t total = Small_Arg_1D;
        
        std::vector<uint32_t> A(total);
        rng.initialize_vector_with_random_values(A);
        std::vector<uint32_t> B(total);
        rng.initialize_vector_with_random_values(B);
        
        std::vector<uint32_t> C_scalar(total);
        std::vector<uint32_t> C_simd(total);

        tensor_bitwise_or(A.data(), B.data(), C_scalar.data(), total);
        tensor_bitwise_or_simd(A.data(), B.data(), C_simd.data(), total);
        
        if(!allclose(C_scalar, C_simd)){
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