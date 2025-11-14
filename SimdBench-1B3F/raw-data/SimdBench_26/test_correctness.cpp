bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t dim1 = Small_Arg_3D;
        size_t dim2 = Small_Arg_3D;
        size_t dim3 = Small_Arg_3D;
        
        std::vector<float> tensor1(dim1 * dim2 * dim3); rng.initialize_vector_with_random_values(tensor1);
        std::vector<float> tensor2(dim1 * dim2 * dim3); rng.initialize_vector_with_random_values(tensor2);
        std::vector<float> dst(dim1 * dim2 * dim3);
        std::vector<float> dst_simd(dim1 * dim2 * dim3);

        tensor3d_sub_round_zero(tensor1.data(), tensor2.data(), dst.data(), dim1, dim2, dim3);
        tensor3d_sub_round_zero_simd(tensor1.data(), tensor2.data(), dst_simd.data(), dim1, dim2, dim3);
        
        if(!allclose(dst, dst_simd)){
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