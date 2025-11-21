bool humaneval_check() {
    bool pass = true;
    pass = pass && (count_nums_simd({}) == 0);
    pass = pass && (count_nums_simd({-1, -2, 0}) == 0);
    pass = pass && (count_nums_simd({1, 1, 2, -2, 3, 4, 5}) == 6);
    pass = pass && (count_nums_simd({1, 6, 9, -6, 0, 1, 5}) == 5);
    pass = pass && (count_nums_simd({1, 100, 98, -7, 1, -1}) == 4);
    pass = pass && (count_nums_simd({12, 23, 34, -45, -56, 0}) == 5);
    pass = pass && (count_nums_simd({-0, 1}) == 1);
    pass = pass && (count_nums_simd({1}) == 1);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> numbers(length);
        for(auto & number: numbers){
            number = rng.randint<int>(-200, 200);
        }
        
        int result = count_nums(numbers);
        int result_simd = count_nums_simd(numbers);
        
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