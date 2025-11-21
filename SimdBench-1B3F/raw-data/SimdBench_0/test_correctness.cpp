
bool correctness_check(int iterations) {
    Random rng;
    for (int  i = 0; i < iterations; i++){
        /* one check */
        size_t length = Small_Arg_1D;
        std::vector<int64_t> src(length); rng.initialize_vector_with_random_values(src);
        std::vector<int64_t> dst(length); rng.initialize_vector_with_random_values(dst);
        std::vector<int64_t> dst_simd(dst); 
        std::vector<uint8_t> mask(length); rng.initialize_vector_with_random_values(mask, true);

        conditional_move(src.data(), dst.data(), reinterpret_cast<const bool*>(mask.data()), length);
        conditional_move_simd(src.data(), dst_simd.data(), reinterpret_cast<const bool*>(mask.data()), length);
        
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
