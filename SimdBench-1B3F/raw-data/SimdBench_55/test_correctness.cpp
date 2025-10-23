bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<float> A(length);
        std::vector<int32_t> control(length);
        std::vector<float> B(length);
        std::vector<float> B_simd(length);
        
        rng.initialize_vector_with_random_values(A);
        rng.initialize_vector_with_random_values(control);

        float a = rng.uniform<float>(), b = rng.uniform<float>();
        float min_val = std::min(a, b);
        float max_val = std::max(a, b) + 0.1; // assert(min_val != max_val)

        conditional_normalize(A.data(), control.data(), B.data(), length, min_val, max_val);
        conditional_normalize_simd(A.data(), control.data(), B_simd.data(), length, min_val, max_val);
        
        if(!allclose(B, B_simd)){
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