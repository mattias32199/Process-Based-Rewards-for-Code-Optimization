bool humaneval_check(){
    bool pass = true;
    pass = pass && (allclose(total_match_simd({}, {}) , {}));
    pass = pass && (allclose(total_match_simd({"hi", "admin"}, {"hi", "hi"}) , {"hi", "hi"}));
    pass = pass && (allclose(total_match_simd({"hi", "admin"}, {"hi", "hi", "admin", "project"}) , {"hi", "admin"}));
    pass = pass && (allclose(total_match_simd({"4"}, {"1", "2", "3", "4", "5"}) , {"4"}));
    pass = pass && (allclose(total_match_simd({"hi", "admin"}, {"hI", "Hi"}) , {"hI", "Hi"}));
    pass = pass && (allclose(total_match_simd({"hi", "admin"}, {"hI", "hi", "hi"}) , {"hI", "hi", "hi"}));
    pass = pass && (allclose(total_match_simd({"hi", "admin"}, {"hI", "hi", "hii"}) , {"hi", "admin"}));
    pass = pass && (allclose(total_match_simd({}, {"this"}) , {}));
    pass = pass && (allclose(total_match_simd({"this"}, {}) , {}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length1 = rng.randint(size_t(0), size_t(128));
        size_t length2 = rng.randint(size_t(0), size_t(128));
        const std::string charset = CHARSET_PRINTABLE;
        std::vector<std::string> lst1(length1);
        std::vector<std::string> lst2(length2);

        for(auto & s: lst1) s = rng.random_string(charset, rng.randint(0,10));
        for(auto & s: lst2) s = rng.random_string(charset, rng.randint(0,20));
              
        auto res_scalar = total_match(lst1, lst2);
        auto res_simd = total_match_simd(lst1, lst2);

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