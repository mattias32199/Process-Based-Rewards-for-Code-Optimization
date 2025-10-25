bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(func_simd(5), {1, 2, 6, 24, 15}));
    pass = pass && (allclose(func_simd(7), {1, 2, 6, 24, 15, 720, 28}));
    pass = pass && (allclose(func_simd(1), {1}));
    pass = pass && (allclose(func_simd(3), {1, 2, 6}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        int n = Small_Arg_1D;
        
        std::vector<int> result = func(n);
        std::vector<int> result_simd = func_simd(n);
        
        if(!allclose(result, result_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed for n=" << n << std::endl;
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
