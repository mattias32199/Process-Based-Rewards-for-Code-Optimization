
bool correctness_check(int iterations) {
    Random rng;
    for (int  i = 0; i < iterations; i++){
        /* one check */
        size_t length = Small_Arg_1D;
        std::vector<float> src(length); rng.initialize_vector_with_random_values(src);
        std::vector<int> indices(length); for(auto& index : indices){ index = rng.randint(size_t(0), length - 1); }
        std::vector<float> dst(length); rng.initialize_vector_with_random_values(dst);
        std::vector<float> dst_simd(dst);

        indexed_access(src.data(), indices.data(), dst.data(), length);
        indexed_access_simd(src.data(), indices.data(), dst_simd.data(), length);
        
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
