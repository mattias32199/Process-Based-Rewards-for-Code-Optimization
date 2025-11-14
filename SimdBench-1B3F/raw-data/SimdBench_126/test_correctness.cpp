bool humaneval_check() {
    bool pass = true;
    pass = pass && fabs(sum_squares_simd({1,2,3}) - 14) < 1e-6;
    pass = pass && fabs(sum_squares_simd({1.0,2,3}) - 14) < 1e-6;
    pass = pass && fabs(sum_squares_simd({1,3,5,7}) - 84) < 1e-6;
    pass = pass && fabs(sum_squares_simd({1.4,4.2,0}) - 29) < 1e-6;
    pass = pass && fabs(sum_squares_simd({-2.4,1,1}) - 6) < 1e-6;
    pass = pass && fabs(sum_squares_simd({100,1,15,2}) - 10230) < 1e-6;
    pass = pass && fabs(sum_squares_simd({10000,10000}) - 200000000) < 1e-6;
    pass = pass && fabs(sum_squares_simd({-1.4,4.6,6.3}) - 75) < 1e-6;
    pass = pass && fabs(sum_squares_simd({-1.4,17.9,18.9,19.9}) - 1086) < 1e-6;
    pass = pass && fabs(sum_squares_simd({0}) - 0) < 1e-6;
    pass = pass && fabs(sum_squares_simd({-1}) - 1) < 1e-6;
    pass = pass && fabs(sum_squares_simd({-1,1,0}) - 2) < 1e-6;
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = rng.randint<size_t>(0, 128);
        std::vector<float> lst(length);
        rng.initialize_vector_with_random_values(lst);
        
        float result = sum_squares(lst);
        float result_simd = sum_squares_simd(lst);
        
        if(fabs(result - result_simd) > 1e-6){
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