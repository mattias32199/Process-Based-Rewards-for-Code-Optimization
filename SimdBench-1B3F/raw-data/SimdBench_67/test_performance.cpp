template<std::vector<int> (*Func)(std::vector<int>)>
static void BM(benchmark::State& state) {
    Random rng;

    size_t length = state.range(0);
    std::vector<int> numbers(length); rng.initialize_vector_with_random_values(numbers);

    for (auto _ : state) {
        std::vector<int> res = Func(numbers);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, rolling_max)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, rolling_max_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();