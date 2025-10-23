template<bool (*Func)(std::vector<int>, int)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng(DEFAULT_SEED);
    std::vector<int> numbers(length);
    rng.initialize_vector_with_random_values(numbers);
    int threshold = rng.randint<int>() % 100;
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(numbers, threshold));
    }
}

BENCHMARK_TEMPLATE(BM, below_threshold)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, below_threshold_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();