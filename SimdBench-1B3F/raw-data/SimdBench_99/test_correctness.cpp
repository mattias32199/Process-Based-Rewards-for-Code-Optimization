bool humaneval_check() {
    bool pass = true;
    pass = pass && (is_simple_power_simd(1, 4)== true);
    pass = pass && (is_simple_power_simd(2, 2)==true);
    pass = pass && (is_simple_power_simd(8, 2)==true);
    pass = pass && (is_simple_power_simd(3, 2)==false);
    pass = pass && (is_simple_power_simd(3, 1)==false);
    pass = pass && (is_simple_power_simd(5, 3)==false);
    pass = pass && (is_simple_power_simd(16, 2)== true);
    pass = pass && (is_simple_power_simd(143214, 16)== false);
    pass = pass && (is_simple_power_simd(4, 2)==true);
    pass = pass && (is_simple_power_simd(9, 3)==true);
    pass = pass && (is_simple_power_simd(16, 4)==true);
    pass = pass && (is_simple_power_simd(24, 2)==false);
    pass = pass && (is_simple_power_simd(128, 4)==false);
    pass = pass && (is_simple_power_simd(12, 6)==false);
    pass = pass && (is_simple_power_simd(1, 1)==true);
    pass = pass && (is_simple_power_simd(1, 12)==true);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        int x = rng.randint<int>(1, 10000);
        int n = rng.randint<int>(1, 16);
        
        auto result = is_simple_power(x, n);
        auto result_simd = is_simple_power_simd(x, n);

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