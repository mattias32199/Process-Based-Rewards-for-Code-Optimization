bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(make_a_pile_simd(5), {5, 7, 9, 11, 13}));
    pass = pass && (allclose(make_a_pile_simd(6), {6, 8, 10, 12, 14, 16}));
    pass = pass && (allclose(make_a_pile_simd(7), {7, 9, 11, 13, 15, 17, 19}));
    pass = pass && (allclose(make_a_pile_simd(10), {10, 12, 14, 16, 18, 20, 22, 24, 26, 28}));
    pass = pass && (allclose(make_a_pile_simd(1), {1}));
    pass = pass && (allclose(make_a_pile_simd(3), {3, 5, 7}));
    pass = pass && (allclose(make_a_pile_simd(4), {4, 6, 8, 10}));
    pass = pass && (allclose(make_a_pile_simd(8), {8, 10, 12, 14, 16, 18, 20, 22}));
    pass = pass && (allclose(make_a_pile_simd(9), {9, 11, 13, 15, 17, 19, 21, 23, 25}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        int n = rng.randint<int>(1, 1024);
        
        std::vector<int> result = make_a_pile(n);
        std::vector<int> result_simd = make_a_pile_simd(n);
        
        if(!allclose(result, result_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed for n=" << n << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    bool res =correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", res ? "1" : "0");
    return 0;
}