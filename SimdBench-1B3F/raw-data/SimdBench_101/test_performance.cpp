template<bool (*Func)(const std::string &)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    const std::string charset = CHARSET_PRINTABLE;
    std::string s = "";
    for(int i=0; i<length; ++i) s.push_back(charset[i%charset.length()]);

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(s));
    }
}

BENCHMARK_TEMPLATE(BM, is_happy)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, is_happy_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();