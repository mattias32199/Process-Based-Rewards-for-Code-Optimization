bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t input_size = Small_Arg_2D;
        size_t kernel_size = rng.randint(size_t(1), input_size);
        size_t output_size = input_size - kernel_size + 1;

        std::vector<double> input(input_size * input_size); rng.initialize_vector_with_random_values(input);
        std::vector<double> kernel(kernel_size * kernel_size); rng.initialize_vector_with_random_values(kernel);
        std::vector<double> output_scalar(output_size * output_size); rng.initialize_vector_with_random_values(output_scalar);
        std::vector<double> output_simd(output_scalar);
        
        simple_conv2d(input.data(), kernel.data(), output_scalar.data(), input_size, kernel_size);
        simple_conv2d_simd(input.data(), kernel.data(), output_simd.data(), input_size, kernel_size);

        if(!allclose(output_scalar, output_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    bool pass = correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}