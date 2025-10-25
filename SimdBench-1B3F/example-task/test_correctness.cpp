
bool correctness_check(int iterations) {
    Random rng;
    for (int  i = 0; i < iterations; i++){
        size_t length = Small_Arg_1D;
        std::vector<float> a(length); rng.initialize_vector_with_random_values(a);
        std::vector<float> b(length); rng.initialize_vector_with_random_values(b);
        std::vector<float> c(length); rng.initialize_vector_with_random_values(c);
        std::vector<float> c_simd(c);

        add_float(a.data(), b.data(), c.data(), length);
        add_float_simd(a.data(), b.data(), c_simd.data(), length);
        
        if(!allclose(c, c_simd)){
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
