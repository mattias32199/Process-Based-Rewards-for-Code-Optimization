template<uint64_t (*Func)(uint64_t)>
static void BM_grid(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    uint64_t n = state.range(0);

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(n));
    }
}

BENCHMARK_TEMPLATE(BM_grid, get_max_triples)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM_grid, get_max_triples_simd)->Name("SIMD")->Medium_Args_1D;
BENCHMARK_MAIN();