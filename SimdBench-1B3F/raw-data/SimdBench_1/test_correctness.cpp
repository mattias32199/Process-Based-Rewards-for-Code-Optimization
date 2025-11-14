
bool correctness_check(int iterations) {
    Random rng;
    for (int  i = 0; i < iterations; i++){
        size_t length = Small_Arg_1D;
        std::vector<int> src(length); for ( auto & element : src) { element = rng.randint(- (1 << 12), 1 << 12); }
        std::vector<int> dst(length); rng.initialize_vector_with_random_values(dst);
        std::vector<int> dst_simd(dst);
        int scale = rng.randint(-100, 100);

        load_modify_store(src.data(), dst.data(), scale, length);
        load_modify_store_simd(src.data(), dst_simd.data(), scale, length);
        
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
