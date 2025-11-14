bool humaneval_check() {
    bool pass = true;
    pass = pass && (largest_divisor_simd(3) == 1);
    pass = pass && (largest_divisor_simd(7) == 1);
    pass = pass && (largest_divisor_simd(10) == 5);
    pass = pass && (largest_divisor_simd(100) == 50);
    pass = pass && (largest_divisor_simd(49) == 7);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        uint64_t n = rng.randint<uint64_t>(1, 1000000);
        uint64_t result = largest_divisor(n);
        uint64_t result_simd = largest_divisor_simd(n);        
        
        if(!allclose(result, result_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
            return false;
        }

    }
    return true;
}


int main() {
    bool pass = humaneval_check() && correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}
