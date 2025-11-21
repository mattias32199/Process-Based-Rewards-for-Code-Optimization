bool humaneval_check(){
    bool res = true;
    res = res && (abs(mean_absolute_deviation_simd({1.0, 2.0, 3.0}) - 2.0/3.0) < 1e-4);
    res = res && (abs(mean_absolute_deviation_simd({1.0, 2.0, 3.0, 4.0}) - 1.0) < 1e-4);
    res = res && (abs(mean_absolute_deviation_simd({1.0, 2.0, 3.0, 4.0, 5.0}) - 6.0/5.0) < 1e-4);
    return res;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;

        std::vector<double> numbers(length); 
        rng.initialize_vector_with_random_values(numbers);
        
        double res_scalar = mean_absolute_deviation(numbers);
        double res_simd = mean_absolute_deviation_simd(numbers);

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