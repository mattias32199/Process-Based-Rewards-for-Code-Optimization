bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t m = Small_Arg_2D;
        size_t n = Small_Arg_2D;
        double scalar = rng.uniform<double>();
        std::vector<double> mat(m * n); rng.initialize_vector_with_random_values(mat);
        std::vector<double> dst(m * n);
        std::vector<double> dst_simd(dst);

        matrix_scalar_mul(mat.data(), scalar, dst.data(), m, n);
        matrix_scalar_mul_simd(mat.data(), scalar, dst_simd.data(), m, n);
        
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