bool humaneval_check() {
    bool pass = true;
    pass = pass && (myexchange_simd({1, 2, 3, 4}, {1, 2, 3, 4}) == "YES");
    pass = pass && (myexchange_simd({1, 2, 3, 4}, {1, 5, 3, 4}) == "NO");
    pass = pass && (myexchange_simd({1, 2, 3, 4}, {2, 1, 4, 3}) == "YES");
    pass = pass && (myexchange_simd({5, 7, 3}, {2, 6, 4}) == "YES");
    pass = pass && (myexchange_simd({5, 7, 3}, {2, 6, 3}) == "NO");
    pass = pass && (myexchange_simd({3, 2, 6, 1, 8, 9}, {3, 5, 5, 1, 1, 1}) == "NO");
    pass = pass && (myexchange_simd({100, 200}, {200, 200}) == "YES");
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        std::vector<int> lst1(rng.randint<size_t>(1, 100));
        std::vector<int> lst2(rng.randint<size_t>(1, 100));
        rng.initialize_vector_with_random_values(lst1);
        rng.initialize_vector_with_random_values(lst2);
        
        std::string result = myexchange(lst1, lst2);
        std::string result_simd = myexchange_simd(lst1, lst2);
        
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