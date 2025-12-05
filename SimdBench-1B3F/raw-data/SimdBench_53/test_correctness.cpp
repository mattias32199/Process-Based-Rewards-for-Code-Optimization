bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<double> A(length);
        std::vector<float> B(length);
        std::vector<float> B_simd(length);
        
        for(auto & item: A) { item = (double)(FLT_MAX) * rng.normal_distribution<double>(0.0, 2.0); }

        double_to_float_saturate(A.data(), B.data(), length);
        double_to_float_saturate_simd(A.data(), B_simd.data(), length);
        
        if(!allclose(B, B_simd)){
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