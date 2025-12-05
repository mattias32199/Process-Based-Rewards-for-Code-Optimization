bool humaneval_check() {
    bool pass = true;
    pass = pass && (allclose(even_odd_palindrome_simd(123), {8, 13}));
    pass = pass && (allclose(even_odd_palindrome_simd(12), {4, 6}));
    pass = pass && (allclose(even_odd_palindrome_simd(3), {1, 2}));
    pass = pass && (allclose(even_odd_palindrome_simd(63), {6, 8}));
    pass = pass && (allclose(even_odd_palindrome_simd(25), {5, 6}));
    pass = pass && (allclose(even_odd_palindrome_simd(19), {4, 6}));
    pass = pass && (allclose(even_odd_palindrome_simd(9), {4, 5}));
    pass = pass && (allclose(even_odd_palindrome_simd(1), {0, 1}));
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        int n = Small_Arg_1D;
        
        std::vector<int> result = even_odd_palindrome(n);
        std::vector<int> result_simd = even_odd_palindrome_simd(n);
        
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