bool humaneval_check(){
    bool pass = true;
    pass = pass && (encrypt_simd("hi") == "lm");
    pass = pass && (encrypt_simd("asdfghjkl") == "ewhjklnop");
    pass = pass && (encrypt_simd("gf") == "kj");
    pass = pass && (encrypt_simd("et") == "ix");
    pass = pass && (encrypt_simd("faewfawefaewg")=="jeiajeaijeiak");
    pass = pass && (encrypt_simd("hellomyfriend")=="lippsqcjvmirh");
    pass = pass && (encrypt_simd("dxzdlmnilfuhmilufhlihufnmlimnufhlimnufhfucufh")=="hbdhpqrmpjylqmpyjlpmlyjrqpmqryjlpmqryjljygyjl");
    pass = pass && (encrypt_simd("a")=="e");
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(0), size_t(128));
        const std::string charset = CHARSET_LOWERCASE;
        std::string s = rng.random_string(charset, length);
              
        auto res_scalar = encrypt(s);
        auto res_simd = encrypt_simd(s);

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