bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t m = Small_Arg_2D;
        size_t n = Small_Arg_2D;
        size_t p = Small_Arg_2D;
        
        std::vector<double> A(m * n);
        for(auto & item: A) { item = rng.normal_distribution<double>(); }
        std::vector<double> B(n * p);
        for(auto & item: B) { item = rng.normal_distribution<double>(); }
        std::vector<double> C(m * p);
        std::vector<double> C_simd(C);

        range_matrix_mul(A.data(), B.data(), C.data(), m, n, p);
        range_matrix_mul_simd(A.data(), B.data(), C_simd.data(), m, n, p);
        
        if(!allclose(C, C_simd)){
            if(ERROR_PRINT) {
                std::cerr << "Correctness check failed!" << std::endl;
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