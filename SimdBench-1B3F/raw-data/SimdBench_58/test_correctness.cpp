bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int32_t> A(length);
        std::vector<uint8_t> cond(length);
        std::vector<float> diff(length);
        std::vector<float> diff_simd(length);
        
        rng.initialize_vector_with_random_values(A);
        rng.initialize_vector_with_random_values(cond, true);

        conditional_diff(A.data(), reinterpret_cast<const bool*>(cond.data()), diff.data(), length);
        conditional_diff_simd(A.data(), reinterpret_cast<const bool*>(cond.data()), diff_simd.data(), length);

        if(!allclose(diff, diff_simd)){
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