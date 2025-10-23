template<int (*Func)(int, int, int)>
static void BM_grid(benchmark::State& state) {
    Random rng(DEFAULT_SEED);

    std::vector<int> large_primes = {20996011, 24036583, 25964951, 30402457, 32582657, 37156667, 42643801, 43112609, 57885161};
    int n = rng.choice(large_primes);
    int x = rng.randint<int>();
    int y = rng.randint<int>();

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(n,x,y));
    }
}

BENCHMARK_TEMPLATE(BM_grid, x_or_y)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM_grid, x_or_y_simd)->Name("SIMD")->Medium_Args_1D;
BENCHMARK_MAIN();