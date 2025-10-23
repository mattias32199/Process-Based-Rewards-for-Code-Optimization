bool humaneval_check() {
    bool pass = true;
    pass = pass && (add_elements_simd({1,-2,-3,41,57,76,87,88,99}, 3) == -4);
    pass = pass && (add_elements_simd({111,121,3,4000,5,6}, 2) == 0);
    pass = pass && (add_elements_simd({11,21,3,90,5,6,7,8,9}, 4) == 125);
    pass = pass && (add_elements_simd({111,21,3,4000,5,6,7,8,9}, 4) == 24);
    pass = pass && (add_elements_simd({1}, 1) == 1);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        int k = rng.randint<int>(1, length);
        std::vector<int> arr(length);
        for(auto & num: arr){
            num = rng.randint(-500, 500);
        }
        
        int result = add_elements(arr, k);
        int result_simd = add_elements_simd(arr, k);
        
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