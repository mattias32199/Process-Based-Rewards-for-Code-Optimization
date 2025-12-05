template<void (*Func)(int16_t *, uint64_t, uint64_t)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    int length = state.range(0);
    std::vector<int16_t> arr(length); rng.initialize_vector_with_random_values(arr);
    int start = rng.randint(0, length - 1);
    int end = rng.randint(start, length - 1);
    
    for (auto _ : state) {
        Func(arr.data(), start, end);
        benchmark::DoNotOptimize(arr.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, range_reverse)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, range_reverse_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();
