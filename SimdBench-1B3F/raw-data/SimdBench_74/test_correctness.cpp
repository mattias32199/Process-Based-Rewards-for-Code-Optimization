bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(factorize_simd(2) , {2}));
    pass = pass && (allclose(factorize_simd(4) , {2, 2}));
    pass = pass && (allclose(factorize_simd(8) , {2, 2, 2}));
    pass = pass && (allclose(factorize_simd(3 * 19) , {3, 19}));
    pass = pass && (allclose(factorize_simd(3 * 19 * 3 * 19) , {3, 3, 19, 19})); 
    pass = pass && (allclose(factorize_simd(3 * 19 * 3 * 19 * 3 * 19) , {3, 3, 3, 19, 19, 19})); 
    pass = pass && (allclose(factorize_simd(3 * 19 * 19 * 19) , {3, 19, 19, 19})); 
    pass = pass && (allclose(factorize_simd(3 * 2 * 3) , {2, 3, 3}));
    return pass;
}

bool correctness_check(int iterations) {

    Random rng;
    for (int i = 0; i < iterations; i++) {
        int n = rng.randint(2, 1000000);
        std::vector<int> result = factorize(n);
        std::vector<int> result_simd = factorize_simd(n);
    
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