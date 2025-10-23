template<std::vector<int> (*Func)(int)>
static void BM(benchmark::State& state) {
    const int n = state.range(0);
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(n));
    }
}

BENCHMARK_TEMPLATE(BM, count_up_to)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, count_up_to_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();