template<bool (*Func)(const std::vector<double> &, double)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);

    size_t length = state.range(0);
    std::vector<double> numbers(length); rng.initialize_vector_with_random_values(numbers);
    double threshold = rng.normal_distribution<double>();

    for (auto _ : state) {
        bool res = Func(numbers, threshold);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, has_close_elements)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, has_close_elements_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();