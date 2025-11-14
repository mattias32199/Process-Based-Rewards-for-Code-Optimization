bool humaneval_check(){
    bool pass = true;
    pass = pass && (solve_simd(1000) == "1");
    pass = pass && (solve_simd(150) == "110");
    pass = pass && (solve_simd(147) == "1100");
    pass = pass && (solve_simd(333) == "1001");
    pass = pass && (solve_simd(963) == "10010");
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        const uint64_t N = rng.randint<uint64_t>();
              
        auto res_scalar = solve(N);
        auto res_simd = solve_simd(N);

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