bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t m = Small_Arg_2D;
        size_t n = Small_Arg_2D;
        
        std::vector<float> mat(m * n); rng.initialize_vector_with_random_values(mat);
        std::vector<float> vec(n); rng.initialize_vector_with_random_values(vec);
        std::vector<float> dst(m);
        std::vector<float> dst_simd(m);

        matrix_vector_mul(mat.data(), vec.data(), dst.data(), m, n);
        matrix_vector_mul_simd(mat.data(), vec.data(), dst_simd.data(), m, n);
        
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