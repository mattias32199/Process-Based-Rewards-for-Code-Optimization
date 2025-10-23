bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        uint32_t mask = rng.randint<uint32_t>();
        std::vector<uint16_t> src1(length); rng.initialize_vector_with_random_values(src1);
        std::vector<uint16_t> src2(length); rng.initialize_vector_with_random_values(src2);
        std::vector<uint16_t> dst(length);
        std::vector<uint16_t> dst_simd(length);

        vector_blend(src1.data(), src2.data(), mask, dst.data(), length);
        vector_blend_simd(src1.data(), src2.data(), mask, dst_simd.data(), length);
        
        if(!allclose(dst, dst_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed! Mask: " << mask << std::endl;
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