
// Benchmarking function
template<void (*Func)(const double*, double*, size_t, size_t, double, double)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    std::vector<double> src(rows * cols); rng.initialize_vector_with_random_values(src);
    std::vector<double> dst(rows * cols); rng.initialize_vector_with_random_values(dst);
    std::vector<double> dst_simd(dst);
    double threshold = rng.uniform(0.0f, 100.0f);
    double scale = rng.uniform(-10.0f, 10.0f);

    for (auto _ : state) {
        Func(src.data(), dst.data(), rows, cols, threshold, scale);
        benchmark::DoNotOptimize(dst.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, conditional_scale)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, conditional_scale_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();
