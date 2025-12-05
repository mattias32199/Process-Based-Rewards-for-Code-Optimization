bool humaneval_check() {
    bool pass = true;
    pass = pass && (special_factorial_simd(4) == 288);
    pass = pass && (special_factorial_simd(5) == 34560);
    pass = pass && (special_factorial_simd(7) == 125411328000 % 100000);
    pass = pass && (special_factorial_simd(1) == 1);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        uint64_t n = rng.randint<uint64_t>(1, 64);
        auto result = special_factorial(n);
        auto result_simd = special_factorial_simd(n);
        
        if (!allclose(result, result_simd)) {
            if (ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
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