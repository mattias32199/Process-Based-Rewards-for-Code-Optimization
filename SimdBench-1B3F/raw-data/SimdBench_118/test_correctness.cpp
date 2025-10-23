bool humaneval_check() {
    bool pass = true;
    pass = pass && (minSubArraySum_simd({2, 3, 4, 1, 2, 4}) == 1);
    pass = pass && (minSubArraySum_simd({-1, -2, -3}) == -6);
    pass = pass && (minSubArraySum_simd({-1, -2, -3, 2, -10}) == -14);
    pass = pass && (minSubArraySum_simd({-9999999999999999}) == -9999999999999999);
    pass = pass && (minSubArraySum_simd({0, 10, 20, 1000000}) == 0);
    pass = pass && (minSubArraySum_simd({-1, -2, -3, 10, -5}) == -6);
    pass = pass && (minSubArraySum_simd({100, -1, -2, -3, 10, -5}) == -6);
    pass = pass && (minSubArraySum_simd({10, 11, 13, 8, 3, 4}) == 3);
    pass = pass && (minSubArraySum_simd({100, -33, 32, -1, 0, -2}) == -33);
    pass = pass && (minSubArraySum_simd({-10}) == -10);
    pass = pass && (minSubArraySum_simd({7}) == 7);
    pass = pass && (minSubArraySum_simd({1, -1}) == -1);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<int64_t> nums(length);
        for(auto& number: nums){
            number = rng.randint<int64_t>(-1024, 1024);
        }
        
        int64_t result = minSubArraySum(nums);
        int64_t result_simd = minSubArraySum_simd(nums);
        
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