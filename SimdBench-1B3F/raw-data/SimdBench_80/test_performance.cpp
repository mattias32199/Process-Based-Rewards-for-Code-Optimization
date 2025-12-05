template<int (*Func)(int)>
static void BM(benchmark::State& state) {
    const size_t n = state.range(0);    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(n));
    }
}

BENCHMARK_TEMPLATE(BM, fizz_buzz)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, fizz_buzz_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();