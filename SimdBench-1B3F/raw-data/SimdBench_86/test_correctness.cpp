bool humaneval_check() {
    bool pass = true;
    pass = pass && (is_palindrome_simd("") == true);
    pass = pass && (is_palindrome_simd("aba") == true);
    pass = pass && (is_palindrome_simd("aaaaa") == true);
    pass = pass && (is_palindrome_simd("zbcd") == false);
    pass = pass && (is_palindrome_simd("xywyx") == true);
    pass = pass && (is_palindrome_simd("xywyz") == false);
    pass = pass && (is_palindrome_simd("xywzx") == false);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    std::vector<std::string> charsets = {
        CHARSET_LOWERCASE, CHARSET_UPPERCASE, CHARSET_DIGITS, CHARSET_ALPHA,
        CHARSET_ALPHANUMERIC, CHARSET_HEX, CHARSET_PRINTABLE
    };
    for (int i = 0; i < iterations; i++) {
        int length = rng.randint<int>(0, 128);
        std::string charset = rng.choice(charsets);
        std::string text = rng.random_string(charset, length);
        if(rng.random_bool()){
            for(int ii = 0; ii < text.size() / 2; ii++) 
                text[ii] = text[text.size() - 1 - ii];
        }
        
        auto result = is_palindrome(text);
        auto result_simd = is_palindrome_simd(text);

        if(!allclose(result, result_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
            return false;
        }
    }
    return true;
}

int main(){
    bool pass = humaneval_check() && correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}
