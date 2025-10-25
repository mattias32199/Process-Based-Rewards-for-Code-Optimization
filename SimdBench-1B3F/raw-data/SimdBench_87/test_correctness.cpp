std::string encode_shift(std::string s){
    std::string out;
    int i;
    for (i=0;i<s.length();i++)
    {
        int w=((int)s[i]+5-(int)'a')%26+(int)'a';   
        out=out+(char)w;
    }
    return out;
}

bool humaneval_check() {
    bool pass = true;
    for (int i=0;i<100;i++)
    {
        int l=10+rand()%11;
        std::string str="";
        for (int j=0;j<l;j++)
        {
            char chr=97+rand()%26;
            str+=chr;
        }

        std::string encoded_str = encode_shift(str);
        pass = pass && (decode_shift_simd(encoded_str) == str);
    }
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
        
        auto result = decode_shift(text);
        auto result_simd = decode_shift_simd(text);

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
