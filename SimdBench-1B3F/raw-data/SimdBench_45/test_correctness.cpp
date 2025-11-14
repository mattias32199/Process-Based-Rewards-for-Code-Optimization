bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t dim1 = Small_Arg_3D;
        size_t dim2 = Small_Arg_3D;
        size_t dim3 = Small_Arg_3D;
        size_t total = dim1 * dim2 * dim3;

        std::vector<float> A(total); rng.initialize_vector_with_random_values(A);
        std::vector<float> B(total); rng.initialize_vector_with_random_values(B);
        std::vector<float> C(total); rng.initialize_vector_with_random_values(C);
        std::vector<float> D(total); rng.initialize_vector_with_random_values(D);

        std::vector<uint8_t> cond1_data(total);
        rng.initialize_vector_with_random_values(cond1_data, true);
        const bool* cond1 = reinterpret_cast<const bool*>(cond1_data.data());
        
        std::vector<uint8_t> cond2_data(total);
        rng.initialize_vector_with_random_values(cond2_data, true);
        const bool* cond2 = reinterpret_cast<const bool*>(cond2_data.data());
        
        std::vector<float> out_scalar(total);
        std::vector<float> out_simd(total);

        tensor_multi_select(A.data(), B.data(), C.data(), D.data(),
                           cond1, cond2, out_scalar.data(),
                           dim1, dim2, dim3);
        tensor_multi_select_simd(A.data(), B.data(), C.data(), D.data(),
                                cond1, cond2, out_simd.data(),
                                dim1, dim2, dim3);
        
        if(!allclose(out_scalar, out_simd)){
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