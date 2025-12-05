template<bool (*Func)(std::vector<int>)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> arr(length);
    rng.initialize_vector_with_random_values(arr);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(arr));
    }
}

BENCHMARK_TEMPLATE(BM, move_one_ball)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, move_one_ball_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();