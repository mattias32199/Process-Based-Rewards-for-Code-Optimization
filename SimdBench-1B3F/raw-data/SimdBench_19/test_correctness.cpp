bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t dim1 = Small_Arg_3D;
        size_t dim2 = Small_Arg_3D;
        size_t dim3 = Small_Arg_3D;
        
        std::vector<int64_t> A(dim1 * dim2 * dim3); for ( auto & element : A) { element = rng.randint<int64_t>(- (1 << 12), 1 << 12); }
        std::vector<int64_t> B(dim1 * dim2 * dim3); for ( auto & element : B) { element = rng.randint<int64_t>(- (1 << 12), 1 << 12); }
        std::vector<int64_t> C(dim1 * dim2 * dim3);
        std::vector<int64_t> C_simd(dim1 * dim2 * dim3);

        tensor_add_3d(A.data(), B.data(), C.data(), dim1, dim2, dim3);
        tensor_add_3d_simd(A.data(), B.data(), C_simd.data(), dim1, dim2, dim3);
        
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