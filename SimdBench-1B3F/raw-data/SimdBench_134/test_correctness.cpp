bool humaneval_check() {
    bool pass = true;
    pass = pass && (double_the_difference_simd({}) == 0);
    pass = pass && (double_the_difference_simd({5, 4}) == 25);
    pass = pass && (double_the_difference_simd({0.1, 0.2, 0.3}) == 0);
    pass = pass && (double_the_difference_simd({-10, -20, -30}) == 0);
    pass = pass && (double_the_difference_simd({-1, -2, 8}) == 0);
    pass = pass && (double_the_difference_simd({0.2, 3, 5}) == 34);
    
    long long odd_sum = 0;
    std::vector<float> lst;
    for (int i = -99; i < 100; i += 2) {
        lst.push_back(i + 0.0f);
        if (i > 0 && i % 2 == 1) odd_sum += i * i;
    }
    pass = pass && (double_the_difference_simd(lst) == odd_sum);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<float> lst(length);
        
        for (auto& num : lst) {
            num = rng.uniform<float>(-100.0f, 100.0f);
            if (rng.randint<int>() % 10 != 0) {
                num = round(num);
            }
        }
        
        long long result = double_the_difference(lst);
        long long result_simd = double_the_difference_simd(lst);
        
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