bool humaneval_check(){
    bool res = true;
    res = res && (solve_simd("AsDf") == "aSdF");
    res = res && (solve_simd("1234") == "4321");
    res = res && (solve_simd("ab") == "AB");
    res = res && (solve_simd("#a@C") == "#A@c");
    res = res && (solve_simd("#AsdfW^45") == "#aSDFw^45");
    res = res && (solve_simd("#6@2") == "2@6#");
    res = res && (solve_simd("#$a^D") == "#$A^d");
    res = res && (solve_simd("#ccc") == "#CCC");
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::string charset;
        if( i % 2 ) charset = CHARSET_PRINTABLE;
        else charset = CHARSET_DIGITS;

        std::string s = rng.random_string(charset, length);

        std::string res_scalar = solve(s);
        std::string res_simd = solve_simd(s);
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