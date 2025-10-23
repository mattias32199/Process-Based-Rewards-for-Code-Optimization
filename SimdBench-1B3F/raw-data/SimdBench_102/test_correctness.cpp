bool humaneval_check(){
    bool pass = true;
    pass = pass && (prime_length_simd("Hello") == true);
    pass = pass && (prime_length_simd("abcdcba") == true);
    pass = pass && (prime_length_simd("kittens") == true);
    pass = pass && (prime_length_simd("orange") == false);
    pass = pass && (prime_length_simd("wow") == true);
    pass = pass && (prime_length_simd("world") == true);
    pass = pass && (prime_length_simd("MadaM") == true);
    pass = pass && (prime_length_simd("Wow") == true);
    pass = pass && (prime_length_simd("") == false);
    pass = pass && (prime_length_simd("HI") == true);
    pass = pass && (prime_length_simd("go") == true);
    pass = pass && (prime_length_simd("gogo") == false);
    pass = pass && (prime_length_simd("aaaaaaaaaaaaaaa") == false);
    pass = pass && (prime_length_simd("Madam") == true);
    pass = pass && (prime_length_simd("M") == false);
    pass = pass && (prime_length_simd("0") == false);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        const std::string charset = CHARSET_PRINTABLE;
        std::string s = rng.random_string(charset, length);
              
        auto res_scalar = prime_length(s);
        auto res_simd = prime_length_simd(s);

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