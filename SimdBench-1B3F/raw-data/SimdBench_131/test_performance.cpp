template<int (*Func)(const std::vector<int> &)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> nums(length);
    rng.initialize_vector_with_random_values(nums);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(nums));
    }
}

BENCHMARK_TEMPLATE(BM, specialFilter)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, specialFilter_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();