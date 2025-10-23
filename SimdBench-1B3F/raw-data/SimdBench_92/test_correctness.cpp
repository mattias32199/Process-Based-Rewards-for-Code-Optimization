bool humaneval_check(){
    bool pass = true;
    pass = pass && (vowels_count_simd("abcde") == 2);
    pass = pass && (vowels_count_simd("Alone") == 3);
    pass = pass && (vowels_count_simd("key") == 2);
    pass = pass && (vowels_count_simd("bye") == 1);
    pass = pass && (vowels_count_simd("keY") == 2);
    pass = pass && (vowels_count_simd("bYe") == 1);
    pass = pass && (vowels_count_simd("ACEDY") == 3);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        const std::string charset = CHARSET_ALPHANUMERIC;
        std::string s = rng.random_string(charset, length);
              
        auto res_scalar = vowels_count(s);
        auto res_simd = vowels_count_simd(s);

        if(!allclose(res_scalar, res_simd)){
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
