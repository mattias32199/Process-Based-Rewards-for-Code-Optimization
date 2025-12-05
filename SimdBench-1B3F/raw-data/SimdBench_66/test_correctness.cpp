bool humaneval_check(){
    bool res = true;
    res = res && (allclose(sum_product_simd({}) ,{0, 1}));
    res = res && (allclose(sum_product_simd({1, 1, 1}), {3, 1}));
    res = res && (allclose(sum_product_simd({100, 0}),{100, 0}));
    res = res && (allclose(sum_product_simd({3, 5, 7}) , {3 + 5 + 7, 3 * 5 * 7}));
    res = res && (allclose(sum_product_simd({10}) ,{10, 10}));
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint(size_t(1), size_t(32));

        std::vector<int16_t> numbers(length);
        for(auto& item:numbers){
            item = rng.randint<int16_t>(1, 4);
        }
        
        std::vector<int64_t> res_scalar = sum_product(numbers);
        std::vector<int64_t> res_simd = sum_product_simd(numbers);

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