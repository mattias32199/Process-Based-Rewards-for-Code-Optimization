template<bool (*Func)(std::vector<int>)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng(DEFAULT_SEED);
    std::vector<int> numbers(length);
    for (size_t j = 0; j < length; j++) {
        numbers[j] = rng.randint( -(1<<10), 1>>10);
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(numbers));
    }
}

BENCHMARK_TEMPLATE(BM, pairs_sum_to_zero)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, pairs_sum_to_zero_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();