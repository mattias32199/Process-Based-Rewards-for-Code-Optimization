template<double (*Func)(std::vector<double>)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);

    size_t length = state.range(0);
    std::vector<double> numbers(length);
    rng.initialize_vector_with_random_values(numbers);

    for (auto _ : state) {
        double res = Func(numbers);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, mean_absolute_deviation)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, mean_absolute_deviation_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();