bool humaneval_check() {
    bool pass = true;
    pass = pass && (is_prime_simd(6) == false);
    pass = pass && (is_prime_simd(101) == true);
    pass = pass && (is_prime_simd(11) == true);
    pass = pass && (is_prime_simd(13441) == true);
    pass = pass && (is_prime_simd(61) == true);
    pass = pass && (is_prime_simd(4) == false);
    pass = pass && (is_prime_simd(1) == false);
    pass = pass && (is_prime_simd(5) == true);
    pass = pass && (is_prime_simd(11) == true);
    pass = pass && (is_prime_simd(17) == true);
    pass = pass && (is_prime_simd(5 * 17) == false);
    pass = pass && (is_prime_simd(11 * 7) == false);
    pass = pass && (is_prime_simd(13441 * 19) == false);
    return pass;
}

bool correctness_check(int iterations) {

    Random rng;
    for (int i = 0; i < iterations; i++) {
        int64_t n = rng.randint(2, 1000000);
        bool result = is_prime(n);
        bool result_simd = is_prime_simd(n);
    
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