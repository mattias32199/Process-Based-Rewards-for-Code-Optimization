bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<float> src1(length); rng.initialize_vector_with_random_values(src1);
        std::vector<float> src2(length); rng.initialize_vector_with_random_values(src2);
        std::vector<float> dst(length);
        std::vector<float> dst_simd(length);

        vector_mul_round_up(src1.data(), src2.data(), dst.data(), length);
        vector_mul_round_up_simd(src1.data(), src2.data(), dst_simd.data(), length);
        
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