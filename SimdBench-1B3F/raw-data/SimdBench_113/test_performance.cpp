template<std::vector<int> (*Func)(int)>
static void BM(benchmark::State& state) {
    //const int n = state.range(0);
    const int n = 1024;
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(n));
    }
}

BENCHMARK_TEMPLATE(BM, even_odd_palindrome)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, even_odd_palindrome_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();