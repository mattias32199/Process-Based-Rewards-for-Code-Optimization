bool humaneval_check() {
    bool pass = true;
    pass = pass && (will_it_fly_simd({3, 2, 3}, 9) == true);
    pass = pass && (will_it_fly_simd({1, 2}, 5) == false);
    pass = pass && (will_it_fly_simd({3}, 5) == true);
    pass = pass && (will_it_fly_simd({3, 2, 3}, 1) == false);
    pass = pass && (will_it_fly_simd({1, 2, 3}, 6) == false);
    pass = pass && (will_it_fly_simd({5}, 5) == true);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int> numbers(length);
        for(auto & element : numbers) {
            element = rng.randint<int>(-50, 100);
        }
        for(int j = 0; j < length / 2; j++) {
            numbers[length - 1 - j] = numbers[j];
        }
        int weight = rng.randint<int>(0, 10000);
        
        bool result = will_it_fly(numbers, weight);
        bool result_simd = will_it_fly_simd(numbers, weight);
        
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
