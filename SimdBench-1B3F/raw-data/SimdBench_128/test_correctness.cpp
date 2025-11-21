bool humaneval_check() {
    bool pass = true;
    pass = pass && allclose(largest_smallest_integers_simd({2, 4, 1, 3, 5, 7}), {0, 1});
    pass = pass && allclose(largest_smallest_integers_simd({2, 4, 1, 3, 5, 7, 0}), {0, 1});
    pass = pass && allclose(largest_smallest_integers_simd({1, 3, 2, 4, 5, 6, -2}), {-2, 1});
    pass = pass && allclose(largest_smallest_integers_simd({4, 5, 3, 6, 2, 7, -7}), {-7, 2});
    pass = pass && allclose(largest_smallest_integers_simd({7, 3, 8, 4, 9, 2, 5, -9}), {-9, 2});
    pass = pass && allclose(largest_smallest_integers_simd({}), {0, 0});
    pass = pass && allclose(largest_smallest_integers_simd({0}), {0, 0});
    pass = pass && allclose(largest_smallest_integers_simd({-1, -3, -5, -6}), {-1, 0});
    pass = pass && allclose(largest_smallest_integers_simd({-1, -3, -5, -6, 0}), {-1, 0});
    pass = pass && allclose(largest_smallest_integers_simd({-6, -4, -4, -3, 1}), {-3, 1});
    pass = pass && allclose(largest_smallest_integers_simd({-6, -4, -4, -3, -100, 1}), {-3, 1});
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> lst(length);
        rng.initialize_vector_with_random_values(lst);
        
        auto result = largest_smallest_integers(lst);
        auto result_simd = largest_smallest_integers_simd(lst);
        
        if(!allclose(result, result_simd)){
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