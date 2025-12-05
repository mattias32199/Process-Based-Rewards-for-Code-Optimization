bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t total = Small_Arg_1D;
        float epsilon = rng.normal_distribution<float>(0.0f, 10.0f);

        std::vector<float> A(total); rng.initialize_vector_with_random_values(A);
        std::vector<float> B(total); rng.initialize_vector_with_random_values(B);
        
        // corner case
        if(i==0){ 
            for(int idx=0; idx<total; ++idx){
                B[idx] = A[idx] + pow(-1, idx) * epsilon;
            }
        } 
        
        std::vector<int8_t> out_scalar(total); rng.initialize_vector_with_random_values(out_scalar);
        std::vector<int8_t> out_simd(out_scalar);

        tensor_ternary_compare(A.data(), B.data(), out_scalar.data(), epsilon, total);
        tensor_ternary_compare_simd(A.data(), B.data(), out_simd.data(), epsilon, total);
        
        if(!allclose(out_scalar, out_simd)){
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