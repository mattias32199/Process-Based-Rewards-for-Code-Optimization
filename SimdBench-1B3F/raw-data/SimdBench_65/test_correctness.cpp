bool humaneval_check(){
    bool res = true;
    res = res && (allclose(intersperse_simd({}, 7), {}));
    res = res && (allclose(intersperse_simd({5, 6, 3, 2}, 8),{5, 8, 6, 8, 3, 8, 2}));
    res = res && (allclose(intersperse_simd({2, 2, 2}, 2),{2, 2, 2, 2, 2}));
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;

        std::vector<int> numbers(length);
        rng.initialize_vector_with_random_values(numbers);
        int delimeter = rng.randint<int>();
        
        std::vector<int> res_scalar = intersperse(numbers, delimeter);
        std::vector<int> res_simd = intersperse_simd(numbers, delimeter);

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