template<std::string (*Func)(const std::string &)>
static void BM(benchmark::State& state) {
    Random rng;
    const std::string charset = CHARSET_LOWERCASE;
    size_t length = state.range(0);
    std::string s = rng.random_string(charset, length);

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(s));
    }
}

BENCHMARK_TEMPLATE(BM, encode)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, encode_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();