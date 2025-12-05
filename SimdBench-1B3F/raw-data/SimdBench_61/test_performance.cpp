template<void (*Func)(const double*, const double*, double*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;

    size_t input_size = state.range(0);
    size_t kernel_size = 8;
    size_t output_size = input_size - kernel_size + 1;

    std::vector<double> input(input_size * input_size); rng.initialize_vector_with_random_values(input);
    std::vector<double> kernel(kernel_size * kernel_size); rng.initialize_vector_with_random_values(kernel);
    std::vector<double> output(output_size * output_size);

    for (auto _ : state) {
        Func(input.data(), kernel.data(), output.data(), input_size, kernel_size);
        benchmark::DoNotOptimize(output.data());
    }
}

BENCHMARK_TEMPLATE(BM, simple_conv2d)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, simple_conv2d_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();