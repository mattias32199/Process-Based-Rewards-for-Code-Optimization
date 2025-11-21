bool humaneval_check(){
    bool pass = true;
    pass = pass && (count_upper_simd("aBCdEf")  == 1);
    pass = pass && (count_upper_simd("abcdefg") == 0);
    pass = pass && (count_upper_simd("dBBE") == 0);
    pass = pass && (count_upper_simd("B")  == 0);
    pass = pass && (count_upper_simd("U")  == 1);
    pass = pass && (count_upper_simd("") == 0);
    pass = pass && (count_upper_simd("EEEE") == 2);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(0), size_t(128));
        const std::string charset = CHARSET_ALPHANUMERIC;
        std::string s = rng.random_string(charset, length);
              
        auto res_scalar = count_upper(s);
        auto res_simd = count_upper_simd(s);

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