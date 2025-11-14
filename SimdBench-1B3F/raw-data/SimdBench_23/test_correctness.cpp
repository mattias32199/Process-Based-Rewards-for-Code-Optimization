bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t m = Small_Arg_2D;
        size_t n = Small_Arg_2D;
        
        std::vector<float> mat(m * n); rng.initialize_vector_with_random_values(mat);
        std::vector<float> mat_simd(mat);

        matrix_normalize_rows(mat.data(), m, n);
        matrix_normalize_rows_simd(mat_simd.data(), m, n);
        
        if(!allclose(mat, mat_simd, 1e-6f, 1e-6f)){
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