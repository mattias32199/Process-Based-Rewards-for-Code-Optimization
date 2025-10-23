bool humaneval_check(){
    bool pass = true;
    pass = pass && (correct_bracketing_simd("()"));
    pass = pass && (correct_bracketing_simd("(()())"));
    pass = pass && (correct_bracketing_simd("()()(()())()"));
    pass = pass && (correct_bracketing_simd("()()((()()())())(()()(()))"));
    pass = pass && (! (correct_bracketing_simd("((()())))")));
    pass = pass && (! (correct_bracketing_simd(")(()")));
    pass = pass && (! (correct_bracketing_simd("(")));
    pass = pass && (! (correct_bracketing_simd("((((")));
    pass = pass && (! (correct_bracketing_simd(")")));
    pass = pass && (! (correct_bracketing_simd("(()")));
    pass = pass && (! (correct_bracketing_simd("()()(()())())(()")));
    pass = pass && (! (correct_bracketing_simd("()()(()())()))()")));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 32);
        const std::string charset = "()";
        std::string brackets = rng.random_string(charset, length);
              
        bool res_scalar = correct_bracketing(brackets);
        bool res_simd = correct_bracketing_simd(brackets);

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
