bool humaneval_check() {
    bool pass = true;    
    pass = pass && (change_base_simd(8, 3) == "22");
    pass = pass && (change_base_simd(9, 3) == "100");
    pass = pass && (change_base_simd(234, 2) == "11101010");
    pass = pass && (change_base_simd(16, 2) == "10000");
    pass = pass && (change_base_simd(8, 2) == "1000");
    pass = pass && (change_base_simd(7, 2) == "111");
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        int x = rng.randint<int>(); if(i==0) x=0;
        int base = rng.randint<int>(2, 9);
        
        auto result = change_base(x, base);
        auto result_simd = change_base_simd(x, base);

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