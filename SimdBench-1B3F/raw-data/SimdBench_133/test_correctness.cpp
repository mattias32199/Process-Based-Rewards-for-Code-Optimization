bool humaneval_check() {
    bool pass = true;
    pass = pass && (x_or_y_simd(7, 34, 12) == 34);
    pass = pass && (x_or_y_simd(15, 8, 5) == 5);
    pass = pass && (x_or_y_simd(3, 33, 5212) == 33);
    pass = pass && (x_or_y_simd(1259, 3, 52) == 3);
    pass = pass && (x_or_y_simd(7919, -1, 12) == -1);
    pass = pass && (x_or_y_simd(3609, 1245, 583) == 583);
    pass = pass && (x_or_y_simd(91, 56, 129) == 129);
    pass = pass && (x_or_y_simd(6, 34, 1234) == 1234);
    pass = pass && (x_or_y_simd(1, 2, 0) == 0);
    pass = pass && (x_or_y_simd(2, 2, 0) == 2);
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        int n = rng.randint<int>(1, 64);
        int x = rng.randint<int>();
        int y = rng.randint<int>();
        auto result = x_or_y(n,x,y);
        auto result_simd = x_or_y_simd(n,x,y);
        
        if (!allclose(result, result_simd)) {
            if (ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
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