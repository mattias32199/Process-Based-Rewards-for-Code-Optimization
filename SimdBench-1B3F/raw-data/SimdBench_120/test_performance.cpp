template<int64_t (*Func)(const std::vector<int16_t>&)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int16_t> lst(length);
    rng.initialize_vector_with_random_values(lst);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(lst));
    }
}

BENCHMARK_TEMPLATE(BM, solution)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, solution_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();