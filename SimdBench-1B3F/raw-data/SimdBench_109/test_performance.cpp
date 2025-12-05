template<int (*Func)(const std::string &)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    const std::string charset = CHARSET_ALPHANUMERIC;
    std::string s = rng.random_string(charset, length);

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(s));
    }
}

BENCHMARK_TEMPLATE(BM, count_upper)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, count_upper_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();