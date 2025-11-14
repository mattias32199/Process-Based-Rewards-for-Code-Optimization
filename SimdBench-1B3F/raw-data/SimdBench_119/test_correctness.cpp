bool humaneval_check() {
    bool pass = true;
    pass = pass && (max_fill_simd({{0,0,1,0}, {0,1,0,0}, {1,1,1,1}}, 1) == 6);
    pass = pass && (max_fill_simd({{0,0,1,1}, {0,0,0,0}, {1,1,1,1}, {0,1,1,1}}, 2) == 5);
    pass = pass && (max_fill_simd({{0,0,0}, {0,0,0}}, 5) == 0);
    pass = pass && (max_fill_simd({{1,1,1,1}, {1,1,1,1}}, 2) == 4);
    pass = pass && (max_fill_simd({{1,1,1,1}, {1,1,1,1}}, 9) == 2);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t rows = Small_Arg_2D;
        size_t cols = Small_Arg_2D;
        int capacity = rng.randint<int>(1, 10);
        
        std::vector<std::vector<int>> grid(rows, std::vector<int>(cols));
        for (auto& row : grid) {
            rng.initialize_vector_with_random_values(row, true);
        }
        
        int result = max_fill(grid, capacity);
        int result_simd = max_fill_simd(grid, capacity);
        
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