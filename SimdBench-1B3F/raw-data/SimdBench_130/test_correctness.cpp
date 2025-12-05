
bool humaneval_check() {
    bool pass = true;
    pass = pass && (sum_squares_simd({1,2,3}) == 6);
    pass = pass && (sum_squares_simd({1,4,9}) == 14);
    pass = pass && (sum_squares_simd({}) == 0);
    pass = pass && (sum_squares_simd({1,1,1,1,1,1,1,1,1}) == 9);
    pass = pass && (sum_squares_simd({-1,-1,-1,-1,-1,-1,-1,-1,-1}) == -3);
    pass = pass && (sum_squares_simd({0}) == 0);
    pass = pass && (sum_squares_simd({-1,-5,2,-1,-5}) == -126);
    pass = pass && (sum_squares_simd({-56,-99,1,0,-2}) == 3030);
    pass = pass && (sum_squares_simd({-1,0,0,0,0,0,0,0,-1}) == 0);
    pass = pass && (sum_squares_simd({-16, -9, -2, 36, 36, 26, -20, 25, -40, 20, -4, 12, -26, 35, 37}) == -14196);
    pass = pass && (sum_squares_simd({-1, -3, 17, -1, -15, 13, -1, 14, -14, -12, -5, 14, -14, 6, 13, 11, 16, 16, 4, 10}) == -1448);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<int> lst(length);
        for(auto & num: lst){
            num = rng.randint<int>(-100, 100);
        }
        
        int result = sum_squares(lst);
        int result_simd = sum_squares_simd(lst);
        
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