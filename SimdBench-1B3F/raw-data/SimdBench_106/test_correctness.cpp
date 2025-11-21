bool humaneval_check(){
    bool pass = true;
    pass = pass && (encode_simd("TEST") == "tgst");
    pass = pass && (encode_simd("Mudasir") == "mWDCSKR");
    pass = pass && (encode_simd("YES") == "ygs");
    pass = pass && (encode_simd("This is a message") == "tHKS KS C MGSSCGG");
    pass = pass && (encode_simd("I DoNt KnOw WhAt tO WrItE") == "k dQnT kNqW wHcT Tq wRkTg");
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(0), size_t(128));
        const std::string charset = CHARSET_LOWERCASE + CHARSET_UPPERCASE + " ";
        std::string s = rng.random_string(charset, length);
              
        auto res_scalar = encode(s);
        auto res_simd = encode_simd(s);

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