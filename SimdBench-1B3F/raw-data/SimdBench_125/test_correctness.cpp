bool humaneval_check() {
    bool pass = true;
    pass = pass && (digits_simd(5) == 5);
    pass = pass && (digits_simd(54) == 5);
    pass = pass && (digits_simd(120) ==1);
    pass = pass && (digits_simd(5014) == 5);
    pass = pass && (digits_simd(98765) == 315);
    pass = pass && (digits_simd(5576543) == 2625);
    pass = pass && (digits_simd(2468) == 0);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {

        uint64_t n = rng.randint<uint64_t>(1);
        auto result = digits(n);
        auto result_simd = digits_simd(n);
        
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