bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;       
        std::vector<int16_t> src(length); rng.initialize_vector_with_random_values(src);

        MinMaxPair res_scalar = min_max_pair(src.data(), length);
        MinMaxPair res_simd = min_max_pair_simd(src.data(), length);
        
        if(!(res_scalar.max_num == res_simd.max_num && 
        res_scalar.min_num == res_simd.min_num)){
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