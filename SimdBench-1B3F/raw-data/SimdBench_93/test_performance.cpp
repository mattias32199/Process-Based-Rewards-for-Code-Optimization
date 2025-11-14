template<int (*Func)(std::string)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    const std::string charset = CHARSET_PRINTABLE;
    std::string s = rng.random_string(charset, length);

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(s));
    }
}

BENCHMARK_TEMPLATE(BM, digitSum)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, digitSum_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();