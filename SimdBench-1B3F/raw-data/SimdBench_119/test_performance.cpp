template<int (*Func)(const std::vector<std::vector<int> > &, int)>
static void BM(benchmark::State& state) {
    const size_t rows = state.range(0);
    const size_t cols = state.range(1);
    Random rng;
    const int capacity = rng.randint<int>(1, 10);
    std::vector<std::vector<int>> grid(rows, std::vector<int>(cols));
    for (auto& row : grid) {
        rng.initialize_vector_with_random_values(row, true);
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(grid, capacity));
    }
}

BENCHMARK_TEMPLATE(BM, max_fill)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, max_fill_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();