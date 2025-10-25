template<std::vector<int> (*Func)(const std::vector<std::vector<int>> &, int)>
static void BM_grid(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    const size_t n = sqrt(state.range(0)); 
    const int k = 32;

    std::vector<int> permutation(n * n);
    for (int i = 0; i < n * n; ++i) {
        permutation[i] = i + 1;
    }
    rng.shuffle(permutation);
    std::vector<std::vector<int>> grid(n, std::vector<int>(n));
    for (size_t idx = 0; idx < permutation.size(); ++idx) {
        size_t row = idx / n;
        size_t col = idx % n;
        grid[row][col] = permutation[idx];
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(grid, k));
    }
}

BENCHMARK_TEMPLATE(BM_grid, minPath)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM_grid, minPath_simd)->Name("SIMD")->Large_Args_1D;
BENCHMARK_MAIN();