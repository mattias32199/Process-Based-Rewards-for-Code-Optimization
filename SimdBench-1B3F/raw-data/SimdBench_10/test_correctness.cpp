bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int64_t> src1(length); for ( auto & element : src1) { element = rng.randint<int64_t>(- (1 << 28), 1 << 28); }
        std::vector<int64_t> src2(length); for ( auto & element : src2) { element = rng.randint<int64_t>(- (1 << 28), 1 << 28); }
        std::vector<int64_t> dst(length); 
        std::vector<int64_t> dst_simd(length);
        int64_t scalar = rng.randint<int64_t>(- (1 << 28), 1 << 28);

        vector_vector_add(src1.data(), src2.data(), dst.data(), scalar, length);
        vector_vector_add_simd(src1.data(), src2.data(), dst_simd.data(), scalar, length);
        
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