template<std::string (*Func)(int64_t, int64_t)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    int64_t a = state.range(0);
    int64_t b = state.range(0)*2;

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(a, b));
    }
}

BENCHMARK_TEMPLATE(BM, rounded_avg)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, rounded_avg_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();