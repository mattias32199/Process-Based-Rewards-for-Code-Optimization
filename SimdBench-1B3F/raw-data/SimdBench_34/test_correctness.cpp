bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        
        int64_t a = rng.randint<int64_t>( - (1 << 20), (1 << 20));
        std::vector<int64_t> x(length); 
        for (auto & item: x) { item = rng.randint<int64_t>( - (1 << 20), (1 << 20)); }
        std::vector<int64_t> y_scalar(length);
        std::vector<int64_t> y_simd(length);
        for(int i=0; i<length; ++i){
            int64_t item = rng.randint<int64_t>( - (1 << 20), (1 << 20));
            y_scalar[i] = item;
            y_simd[i] = item;
        }

        axm_abs(length, a, x.data(), y_scalar.data());
        axm_abs_simd(length, a, x.data(), y_simd.data());
        
        if(!allclose(y_scalar, y_simd)){
            if(ERROR_PRINT) {
                std::cerr << "Correctness check failed!" << std::endl;
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
