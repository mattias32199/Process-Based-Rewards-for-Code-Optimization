bool humaneval_check() {
    bool pass = true;
    pass = pass && (is_sorted_simd({5}) == true);
    pass = pass && (is_sorted_simd({1, 2, 3, 4, 5}) == true);
    pass = pass && (is_sorted_simd({1, 3, 2, 4, 5}) == false);
    pass = pass && (is_sorted_simd({1, 2, 3, 4, 5, 6}) == true);
    pass = pass && (is_sorted_simd({1, 2, 3, 4, 5, 6, 7}) == true);
    pass = pass && (is_sorted_simd({1, 3, 2, 4, 5, 6, 7}) == false);
    pass = pass && (is_sorted_simd({}) == true);
    pass = pass && (is_sorted_simd({1}) == true);
    pass = pass && (is_sorted_simd({3, 2, 1}) == false);
    pass = pass && (is_sorted_simd({1, 2, 2, 2, 3, 4}) == false);
    pass = pass && (is_sorted_simd({1, 2, 3, 3, 3, 4}) == false);
    pass = pass && (is_sorted_simd({1, 2, 2, 3, 3, 4}) == true);
    pass = pass && (is_sorted_simd({1, 2, 3, 4}) == true);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(0), size_t(1024));
        std::vector<int> lst(length);
        if(rng.randint<int>() % 2) {

            for(size_t j = 0; j < length; j++) {
                lst[j] = static_cast<int>(j);
            }

            if(rng.randint<int>() % 2) {
                for(size_t j = 1; j < length; j++) {
                    if(rng.randint<int>() % 3 == 0) { 
                        lst[j] = lst[j-1];
                    }
                }
            }
        } else {
            rng.initialize_vector_with_random_values(lst);
        }
        
        bool result = is_sorted(lst);
        bool result_simd = is_sorted_simd(lst);
        
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