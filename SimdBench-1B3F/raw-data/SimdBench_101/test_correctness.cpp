bool humaneval_check(){
    bool pass = true;
    pass = pass && (is_happy_simd("a") == false );
    pass = pass && (is_happy_simd("aa") == false );
    pass = pass && (is_happy_simd("abcd") == true );
    pass = pass && (is_happy_simd("aabb") == false );
    pass = pass && (is_happy_simd("adb") == true );
    pass = pass && (is_happy_simd("xyy") == false );
    pass = pass && (is_happy_simd("iopaxpoi") == true );
    pass = pass && (is_happy_simd("iopaxioi") == false );
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(0), size_t(10));
        const std::string charset = CHARSET_PRINTABLE;
        std::string s = rng.random_string(charset, length);
              
        auto res_scalar = is_happy(s);
        auto res_simd = is_happy_simd(s);

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