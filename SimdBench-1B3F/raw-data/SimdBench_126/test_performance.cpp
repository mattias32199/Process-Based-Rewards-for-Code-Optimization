template<uint64_t (*Func)(const std::vector<float> &)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<float> lst(length);
    rng.initialize_vector_with_random_values(lst);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(lst));
    }
}

BENCHMARK_TEMPLATE(BM, sum_squares)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, sum_squares_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();