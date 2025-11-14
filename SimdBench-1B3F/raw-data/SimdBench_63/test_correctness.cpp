bool humaneval_check(){
    bool res = true;
    res = res && (below_zero_simd({}) == false);
    res = res && (below_zero_simd({1, 2, -3, 1, 2, -3}) == false);
    res = res && (below_zero_simd({1, 2, -4, 5, 6}) == true);
    res = res && (below_zero_simd({1, -1, 2, -2, 5, -5, 4, -4}) == false);
    res = res && (below_zero_simd({1, -1, 2, -2, 5, -5, 4, -5}) == true);
    res = res && (below_zero_simd({1, -2, 2, -2, 5, -5, 4, -4}) == true);
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;

        std::vector<int> operations(length);
        for(auto& item:operations){
            item = rng.randint<int>(-10.0, 20.0);
        }
        
        bool res_scalar = below_zero(operations);
        bool res_simd = below_zero_simd(operations);

        if(!allclose(res_scalar, res_simd)){
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