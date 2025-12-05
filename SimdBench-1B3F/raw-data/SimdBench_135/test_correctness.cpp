bool humaneval_check() {
    bool pass = true;
    pass = pass && allclose(compare_simd({1,2,3,4,5,1}, {1,2,3,4,2,-2}), {0,0,0,0,3,3});
    pass = pass && allclose(compare_simd({0,5,0,0,0,4}, {4,1,1,0,0,-2}), {4,4,1,0,0,6});
    pass = pass && allclose(compare_simd({1,2,3,4,5,1}, {1,2,3,4,2,-2}), {0,0,0,0,3,3});
    pass = pass && allclose(compare_simd({0,0,0,0,0,0}, {0,0,0,0,0,0}), {0,0,0,0,0,0});
    pass = pass && allclose(compare_simd({1,2,3}, {-1,-2,-3}), {2,4,6});
    pass = pass && allclose(compare_simd({1,2,3,5}, {-1,2,3,4}), {2,0,0,1});
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> game(length);
        std::vector<int> guess(length);
        for( int i=0; i< length; ++i){
            game[i] = rng.randint<int>(-1000, 1000);
            guess[i] = rng.randint<int>(-1000, 1000);
        }
        auto result = compare(game, guess);
        auto result_simd = compare_simd(game, guess);
        
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