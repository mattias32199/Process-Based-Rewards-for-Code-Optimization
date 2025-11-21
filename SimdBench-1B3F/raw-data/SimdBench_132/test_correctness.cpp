bool humaneval_check() {
    bool pass = true;
    pass = pass && (get_max_triples_simd(5) == 1);
    pass = pass && (get_max_triples_simd(6) == 4);
    pass = pass && (get_max_triples_simd(10) == 36);
    pass = pass && (get_max_triples_simd(100) == 53361);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        uint64_t n = rng.randint<uint64_t>(1, 64);
        auto result = get_max_triples(n);
        auto result_simd = get_max_triples_simd(n);
        
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