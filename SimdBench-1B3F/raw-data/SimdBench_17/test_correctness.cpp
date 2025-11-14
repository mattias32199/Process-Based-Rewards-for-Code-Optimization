bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<uint16_t> src1(length); rng.initialize_vector_with_random_values(src1);
        std::vector<uint16_t> src2(length); rng.initialize_vector_with_random_values(src2);
        std::vector<uint16_t> dst(length);
        std::vector<uint16_t> dst_simd(length);

        if (i == 0) {
            src1[0] = UINT16_MAX;
            src2[0] = 1;
        }

        saturating_add(src1.data(), src2.data(), dst.data(), length);
        saturating_add_simd(src1.data(), src2.data(), dst_simd.data(), length);
        
        if(!allclose(dst, dst_simd)){
            if(ERROR_PRINT) {
                std::cerr << "Correctness check failed!" << std::endl;
                std::cerr << "First mismatch at index: ";
                for (size_t j = 0; j < length; j++) {
                    if (dst[j] != dst_simd[j]) {
                        std::cerr << j << " (scalar: " << dst[j] 
                                  << ", SIMD: " << dst_simd[j] 
                                  << ", src1: " << src1[j]
                                  << ", src2: " << src2[j] << ")" << std::endl;
                        break;
                    }
                }
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