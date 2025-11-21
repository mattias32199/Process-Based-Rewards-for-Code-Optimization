template<std::vector<int> (*Func)(int)>
static void BM(benchmark::State& state) {
    const int n = state.range(0);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(n));
    }
}

BENCHMARK_TEMPLATE(BM, func)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, func_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();