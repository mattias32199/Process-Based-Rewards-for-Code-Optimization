bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<float> A(length);
        std::vector<float> B(length);
        std::vector<int32_t> C(length);
        std::vector<int32_t> C_simd(length);
        
        for (auto & element: A) { element = ((int64_t)1 << 32) * rng.normal_distribution<float>(0.0, 1.0); }
        rng.initialize_vector_with_random_values(B);

        float_div_round(A.data(), B.data(), C.data(), length);
        float_div_round_simd(A.data(), B.data(), C_simd.data(), length);
        
        if(!allclose(C, C_simd)){
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