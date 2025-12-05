template<int (*Func)(std::string)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    const std::string charset = CHARSET_ALPHANUMERIC;
    std::string s = rng.random_string(charset, length);

    for (auto _ : state) {
        int res = Func(s);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, vowels_count)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, vowels_count_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();