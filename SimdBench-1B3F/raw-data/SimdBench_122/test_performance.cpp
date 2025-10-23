template<bool (*Func)(const std::vector<int> &)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng(DEFAULT_SEED);
    std::vector<int> lst(length);
    for(size_t i = 0; i < length; i++) {
        lst[i] = static_cast<int>(i);
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(lst));
    }
}

BENCHMARK_TEMPLATE(BM, is_sorted)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, is_sorted_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();