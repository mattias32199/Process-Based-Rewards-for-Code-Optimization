bool humaneval_check() {
    bool pass = true;
    pass = pass && (solution_simd({5, 8, 7, 1}) == 12);
    pass = pass && (solution_simd({3, 3, 3, 3, 3}) == 9);
    pass = pass && (solution_simd({30, 13, 24, 321}) == 0);
    pass = pass && (solution_simd({5, 9}) == 5);
    pass = pass && (solution_simd({2, 4, 8}) == 0);
    pass = pass && (solution_simd({30, 13, 23, 32}) == 23);
    pass = pass && (solution_simd({3, 13, 2, 9}) == 3);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int16_t> lst(length);
        rng.initialize_vector_with_random_values(lst);
        
        int result = solution(lst);
        int result_simd = solution_simd(lst);
        
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