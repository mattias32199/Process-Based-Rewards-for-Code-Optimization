bool humaneval_check() {
    bool pass = true;
    pass = pass && (search_simd({5, 5, 5, 5, 1}) == 1);
    pass = pass && (search_simd({4, 1, 4, 1, 4, 4}) == 4);
    pass = pass && (search_simd({3, 3}) == -1);
    pass = pass && (search_simd({8, 8, 8, 8, 8, 8, 8, 8}) == 8);
    pass = pass && (search_simd({2, 3, 3, 2, 2}) == 2);
    pass = pass && (search_simd({2, 7, 8, 8, 4, 8, 7, 3, 9, 6, 5, 10, 4, 3, 6, 7, 1, 7, 4, 10, 8, 1}) == 1);
    pass = pass && (search_simd({3, 2, 8, 2}) == 2);
    pass = pass && (search_simd({6, 7, 1, 8, 8, 10, 5, 8, 5, 3, 10}) == 1);
    pass = pass && (search_simd({8, 8, 3, 6, 5, 6, 4}) == -1);
    pass = pass && (search_simd({6, 9, 6, 7, 1, 4, 7, 1, 8, 8, 9, 8, 10, 10, 8, 4, 10, 4, 10, 1, 2, 9, 5, 7, 9}) == 1);
    pass = pass && (search_simd({1, 9, 10, 1, 3}) == 1);
    pass = pass && (search_simd({6, 9, 7, 5, 8, 7, 5, 3, 7, 5, 10, 10, 3, 6, 10, 2, 8, 6, 5, 4, 9, 5, 3, 10}) == 5);
    pass = pass && (search_simd({1}) == 1);
    pass = pass && (search_simd({8, 8, 10, 6, 4, 3, 5, 8, 2, 4, 2, 8, 4, 6, 10, 4, 2, 1, 10, 2, 1, 1, 5}) == 4);
    pass = pass && (search_simd({2, 10, 4, 8, 2, 10, 5, 1, 2, 9, 5, 5, 6, 3, 8, 6, 4, 10}) == 2);
    pass = pass && (search_simd({1, 6, 10, 1, 6, 9, 10, 8, 6, 8, 7, 3}) == 1);
    pass = pass && (search_simd({9, 2, 4, 1, 5, 1, 5, 2, 5, 7, 7, 7, 3, 10, 1, 5, 4, 2, 8, 4, 1, 9, 10, 7, 10, 2, 8, 10, 9, 4}) == 4);
    pass = pass && (search_simd({2, 6, 4, 2, 8, 7, 5, 6, 4, 10, 4, 6, 3, 7, 8, 8, 3, 1, 4, 2, 2, 10, 7}) == 4);
    pass = pass && (search_simd({9, 8, 6, 10, 2, 6, 10, 2, 7, 8, 10, 3, 8, 2, 6, 2, 3, 1}) == 2);
    pass = pass && (search_simd({5, 5, 3, 9, 5, 6, 3, 2, 8, 5, 6, 10, 10, 6, 8, 4, 10, 7, 7, 10, 8}) == -1);
    pass = pass && (search_simd({10}) == -1);
    pass = pass && (search_simd({9, 7, 7, 2, 4, 7, 2, 10, 9, 7, 5, 7, 2}) == 2);
    pass = pass && (search_simd({5, 4, 10, 2, 1, 1, 10, 3, 6, 1, 8}) == 1);
    pass = pass && (search_simd({7, 9, 9, 9, 3, 4, 1, 5, 9, 1, 2, 1, 1, 10, 7, 5, 6, 7, 6, 7, 7, 6}) == 1);
    pass = pass && (search_simd({3, 10, 10, 9, 2}) == -1);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        
        int result = search(numbers);
        int result_simd = search_simd(numbers);
        
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
