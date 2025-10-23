bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(count_up_to_simd(5), {2,3}));
    pass = pass && (allclose(count_up_to_simd(6), {2,3,5}));
    pass = pass && (allclose(count_up_to_simd(7), {2,3,5}));
    pass = pass && (allclose(count_up_to_simd(10), {2,3,5,7}));
    pass = pass && (allclose(count_up_to_simd(0), {}));
    pass = pass && (allclose(count_up_to_simd(22), {2,3,5,7,11,13,17,19}));
    pass = pass && (allclose(count_up_to_simd(1), {}));
    pass = pass && (allclose(count_up_to_simd(18), {2,3,5,7,11,13,17}));
    pass = pass && (allclose(count_up_to_simd(47), {2,3,5,7,11,13,17,19,23,29,31,37,41,43}));
    pass = pass && (allclose(count_up_to_simd(101), {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        int n = rng.randint<int>(2, 1 << 14);
        
        std::vector<int> result = count_up_to(n);
        std::vector<int> result_simd = count_up_to_simd(n);
        
        if(!allclose(result, result_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed for n=" << n << std::endl;
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