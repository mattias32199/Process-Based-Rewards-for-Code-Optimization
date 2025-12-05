bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int8_t> A(length); rng.initialize_vector_with_random_values(A);
        std::vector<int8_t> B(length); rng.initialize_vector_with_random_values(B);

        int64_t diff_scalar = squarediff(A.data(), B.data(), length);
        int64_t diff_simd =  squarediff_simd(A.data(), B.data(), length);
        
        if(diff_scalar != diff_simd){
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