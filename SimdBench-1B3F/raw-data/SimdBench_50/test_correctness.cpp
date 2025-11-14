bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<float> src(length);
        for (auto & element: src) { element = ((int64_t)1 << 32) * rng.normal_distribution<float>(0.0, 1.0); }
        std::vector<int32_t> dst(length); rng.initialize_vector_with_random_values(dst);
        std::vector<int32_t> dst_simd(dst);
        
        float_to_int_saturate(src.data(), dst.data(), length);
        float_to_int_saturate_simd(src.data(), dst_simd.data(), length);
        
        if (!allclose(dst, dst_simd)) {
            if (ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
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