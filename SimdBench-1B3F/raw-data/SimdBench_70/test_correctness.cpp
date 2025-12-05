bool humaneval_check(){
    bool res = true;
    res = res && (greatest_common_divisor_simd(3, 7) == 1);
    res = res && (greatest_common_divisor_simd(10, 15) == 5);
    res = res && (greatest_common_divisor_simd(49, 14) == 7);
    res = res && (greatest_common_divisor_simd(144, 60) == 12);
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        uint64_t a = rng.randint<uint64_t>(1);
        uint64_t b = rng.randint<uint64_t>(1);
        
        uint64_t result = greatest_common_divisor(a, b);
        uint64_t result_simd = greatest_common_divisor_simd(a, b);
        
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