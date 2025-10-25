bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t m = Small_Arg_2D;
        size_t n = Small_Arg_2D;
        
        std::vector<double> mat(m * n);  rng.initialize_vector_with_random_values(mat);

        double avg = matrix_average(mat.data(), m, n);
        double avg_simd = matrix_average_simd(mat.data(), m, n);
        
        if(!allclose(avg, avg_simd)){
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