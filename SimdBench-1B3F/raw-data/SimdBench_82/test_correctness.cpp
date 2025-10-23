bool humaneval_check() {
    bool pass = true;
    pass = pass && (triples_sum_to_zero_simd({1, 3, 5, 0}) == false);
    pass = pass && (triples_sum_to_zero_simd({1, 3, 5, -1}) == false);
    pass = pass && (triples_sum_to_zero_simd({1, 3, -2, 1}) == true);
    pass = pass && (triples_sum_to_zero_simd({1, 2, 3, 7}) == false);
    pass = pass && (triples_sum_to_zero_simd({1, 2, 5, 7}) == false);
    pass = pass && (triples_sum_to_zero_simd({2, 4, -5, 3, 9, 7}) == true);
    pass = pass && (triples_sum_to_zero_simd({1}) == false);
    pass = pass && (triples_sum_to_zero_simd({1, 3, 5, -100}) == false);
    pass = pass && (triples_sum_to_zero_simd({100, 3, 5, -100}) == false);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_2D;
        std::vector<int> numbers(length);
        for(auto& number : numbers) {
            number = rng.randint(-100, 100);
        }
        
        bool result = triples_sum_to_zero(numbers);
        bool result_simd = triples_sum_to_zero_simd(numbers);
        
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
