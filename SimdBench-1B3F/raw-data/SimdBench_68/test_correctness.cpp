bool humaneval_check(){
    bool res = true;
    res = res && (string_xor_simd("111000", "101010") == "010010");
    res = res && (string_xor_simd("1", "1") == "0");
    res = res && (string_xor_simd("0101", "0000") == "0101");
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        const std::string charset = "01";
        std::string a = rng.random_string(charset, length);
        std::string b = rng.random_string(charset, length);
              
        std::string res_scalar = string_xor(a, b);
        std::string res_simd = string_xor_simd(a, b);

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