template<std::vector<int> (*Func)(int)>
static void BM(benchmark::State& state) {
    const int n = state.range(0);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(n));
    }
}

BENCHMARK_TEMPLATE(BM, make_a_pile)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, make_a_pile_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();