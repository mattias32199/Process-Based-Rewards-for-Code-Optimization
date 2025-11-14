bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t total = Small_Arg_1D;
        
        std::vector<uint16_t> A(total); rng.initialize_vector_with_random_values(A);
        std::vector<uint16_t> B(total); rng.initialize_vector_with_random_values(B);
        
        std::vector<uint16_t> C_scalar(total);
        std::vector<uint16_t> C_simd(total);

        tensor_bit_op(A.data(), B.data(), C_scalar.data(), total);
        tensor_bit_op_simd(A.data(), B.data(), C_simd.data(), total);

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