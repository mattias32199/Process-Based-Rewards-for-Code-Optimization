bool humaneval_check(){
    bool res = true;
    res = res && (hex_key_simd("AB") == 1);
    res = res && (hex_key_simd("1077E") == 2);
    res = res && (hex_key_simd("ABED1A33") == 4);
    res = res && (hex_key_simd("2020") == 2);
    res = res && (hex_key_simd("123456789ABCDEF0") == 6);
    res = res && (hex_key_simd("112233445566778899AABBCCDDEEFF00") == 12);
    res = res && (hex_key_simd("") == 0);
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(0), size_t(128));
        const std::string charset = CHARSET_HEX;
        std::string num = rng.random_string(charset, length);
              
        auto res_scalar = hex_key(num);
        auto res_simd = hex_key_simd(num);

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