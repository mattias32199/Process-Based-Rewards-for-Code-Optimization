bool humaneval_check(){
    bool pass = true;
    pass = pass && (digitSum_simd("") == 0);
    pass = pass && (digitSum_simd("abAB") == 131);
    pass = pass && (digitSum_simd("abcCd") == 67);
    pass = pass && (digitSum_simd("helloE") == 69);
    pass = pass && (digitSum_simd("woArBld") == 131);
    pass = pass && (digitSum_simd("aAaaaXa") == 153);
    pass = pass && (digitSum_simd(" How are yOu?") == 151);
    pass = pass && (digitSum_simd("You arE Very Smart") == 327);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(0), size_t(128));
        const std::string charset = CHARSET_PRINTABLE;
        std::string s = rng.random_string(charset, length);
              
        auto res_scalar = digitSum(s);
        auto res_simd = digitSum_simd(s);

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
