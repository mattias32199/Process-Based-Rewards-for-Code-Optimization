bool humaneval_check(){
    bool res = true;
    res = res && (longest_simd({}) == "");
    res = res && (longest_simd({"x", "y", "z"}) == "x");
    res = res && (longest_simd({"x", "yyy", "zzzz", "www", "kkkk", "abc"}) == "zzzz");
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        const std::string charset = CHARSET_ALPHANUMERIC;
        std::vector<std::string> strings(length);
        for(auto& item: strings){
            item = rng.random_string(charset, rng.randint(size_t(0), size_t(64)));
        }
              
        std::string res_scalar = longest(strings);
        std::string res_simd = longest_simd(strings);

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