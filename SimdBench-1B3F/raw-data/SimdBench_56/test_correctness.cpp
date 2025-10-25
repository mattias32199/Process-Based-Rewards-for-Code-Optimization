bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<uint32_t> A(length);
        std::vector<float> B(length);
        std::vector<float> B_simd(length);
        
        rng.initialize_vector_with_random_values(A);

        int_to_float_sqrt(A.data(), B.data(), length);
        int_to_float_sqrt_simd(A.data(), B_simd.data(), length);
        
        if(!allclose(B, B_simd)){
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