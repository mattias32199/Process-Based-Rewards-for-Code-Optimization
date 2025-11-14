bool humaneval_check() {
    bool pass = true;
    pass = pass && (fizz_buzz_simd(50) == 0);
    pass = pass && (fizz_buzz_simd(78) == 2);
    pass = pass && (fizz_buzz_simd(79) == 3);
    pass = pass && (fizz_buzz_simd(100) == 3);
    pass = pass && (fizz_buzz_simd(200) == 6);
    pass = pass && (fizz_buzz_simd(4000) == 192);
    pass = pass && (fizz_buzz_simd(10000) == 639);
    pass = pass && (fizz_buzz_simd(100000) == 8026);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        int n = rng.randint<int>(0, 10000);
        
        auto result = fizz_buzz(n);
        auto result_simd = fizz_buzz_simd(n);

        if(!allclose(result, result_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
            return false;
        }
    }
    return true;
}

int main(){
    bool pass = humaneval_check() && correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}