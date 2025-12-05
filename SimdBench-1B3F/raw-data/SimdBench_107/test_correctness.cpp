bool humaneval_check() {
    bool pass = true;
    pass = pass && (check_dict_case_simd({{"p","pineapple"}, {"b","banana"}}) == true);
    pass = pass && (check_dict_case_simd({{"p","pineapple"}, {"A","banana"}, {"B","banana"}}) == false);
    pass = pass && (check_dict_case_simd({{"p","pineapple"}, {"5","banana"}, {"a","apple"}}) == false);
    pass = pass && (check_dict_case_simd({{"Name","John"}, {"Age","36"}, {"City","Houston"}}) == false);
    pass = pass && (check_dict_case_simd({{"STATE","NC"}, {"ZIP","12345"} }) == true  );
    pass = pass && (check_dict_case_simd({{"fruit","Orange"}, {"taste","Sweet"} }) == true  );
    pass = pass && (check_dict_case_simd({}) == false);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        int type = rng.randint<int>() % 6;
        std::string charset;
        if(type==0) charset = CHARSET_LOWERCASE;
        else if(type==1) charset = CHARSET_UPPERCASE;
        else if(type==2) charset = CHARSET_ALPHANUMERIC;
        else if(type==3) charset = CHARSET_PRINTABLE;
        else if(type==4) charset = CHARSET_HEX;
        else charset = CHARSET_ALPHA;

        std::map<std::string, std::string> dict;
        for (size_t j = 0; j < length; j++) {
            std::string key = rng.random_string(charset, rng.randint<size_t>(1, 20));
            std::string value = rng.random_string(charset, rng.randint<size_t>(1, 20));
            dict[key] = value;
        }
        
        bool result = check_dict_case(dict);
        bool result_simd = check_dict_case_simd(dict);
        
        if(result != result_simd){
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