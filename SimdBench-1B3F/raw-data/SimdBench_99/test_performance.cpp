template<bool (*Func)(uint64_t, uint64_t)>
static void BM(benchmark::State& state) {
    Random rng;
    uint64_t n = rng.randint<int>(2, 3);
    uint64_t x = pow(n, 30);  
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(x, n));
    }
}

BENCHMARK_TEMPLATE(BM, is_simple_power)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, is_simple_power_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();