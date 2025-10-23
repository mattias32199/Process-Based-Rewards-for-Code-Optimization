bool humaneval_check(){
    bool pass = true;
    pass = pass && (rounded_avg_simd(1, 5) == "11");
    pass = pass && (rounded_avg_simd(7, 13) == "1010");
    pass = pass && (rounded_avg_simd(964,977) == "1111001010");
    pass = pass && (rounded_avg_simd(996,997) == "1111100100");
    pass = pass && (rounded_avg_simd(560,851) == "1011000001");  
    pass = pass && (rounded_avg_simd(185,546) == "101101101");
    pass = pass && (rounded_avg_simd(362,496) == "110101101");
    pass = pass && (rounded_avg_simd(350,902) == "1001110010");
    pass = pass && (rounded_avg_simd(197,233) == "11010111");
    pass = pass && (rounded_avg_simd(7, 5) == "-1");
    pass = pass && (rounded_avg_simd(5, 1) == "-1");
    pass = pass && (rounded_avg_simd(5, 5) == "101");
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        int64_t a = rng.randint<int64_t>(1, 10000);
        int64_t b = rng.randint<int64_t>(1, 10000);
              
        auto res_scalar = rounded_avg(a, b);
        auto res_simd = rounded_avg_simd(a, b);

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