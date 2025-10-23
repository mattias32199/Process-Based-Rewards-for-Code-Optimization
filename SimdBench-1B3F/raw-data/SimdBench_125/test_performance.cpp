template<uint64_t (*Func)(uint64_t)>
static void BM_grid(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    uint64_t n = rng.randint<uint64_t>(1);

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(n));
    }
}

BENCHMARK_TEMPLATE(BM_grid, digits)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM_grid, digits_simd)->Name("SIMD")->Large_Args_1D;
BENCHMARK_MAIN();