bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<uint32_t> src(length);
        std::vector<uint64_t> dst(length);
        std::vector<uint64_t> dst_simd(length);
        
        rng.initialize_vector_with_random_values(src);

        widening_uint(src.data(), dst.data(), length);
        widening_uint_simd(src.data(), dst_simd.data(), length);

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