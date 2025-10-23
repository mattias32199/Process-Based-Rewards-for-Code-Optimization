bool humaneval_check() {
    bool pass = true;
    pass = pass && (move_one_ball_simd({3, 4, 5, 1, 2}) == true);
    pass = pass && (move_one_ball_simd({3, 5, 10, 1, 2}) == true);
    pass = pass && (move_one_ball_simd({4, 3, 1, 2}) == false);
    pass = pass && (move_one_ball_simd({3, 5, 4, 1, 2}) == false);
    pass = pass && (move_one_ball_simd({}) == true);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> arr(length);
        rng.initialize_vector_with_random_values(arr);
        
        bool result = move_one_ball(arr);
        bool result_simd = move_one_ball_simd(arr);
        
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