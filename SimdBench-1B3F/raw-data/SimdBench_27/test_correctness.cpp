bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t m = Small_Arg_2D;
        size_t n = Small_Arg_2D;
        
        std::vector<double> mat1(m * n); rng.initialize_vector_with_random_values(mat1);
        std::vector<double> mat2(m * n); rng.initialize_vector_with_random_values(mat2);
        std::vector<double> dst(m * n);
        std::vector<double> dst_simd(m * n);

        matrix_hadamard_product(mat1.data(), mat2.data(), dst.data(), m, n);
        matrix_hadamard_product_simd(mat1.data(), mat2.data(), dst_simd.data(), m, n);
        
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