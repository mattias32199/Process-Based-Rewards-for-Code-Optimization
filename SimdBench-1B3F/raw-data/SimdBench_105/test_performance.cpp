template<std::string (*Func)(const std::string &)>
static void BM(benchmark::State& state) {
    Random rng;
    const std::string charset = CHARSET_LOWERCASE;
    size_t length = state.range(0);
    std::string s = "";
    for(int i=0; i<length; ++i) s.push_back(charset[i%charset.length()]);

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(s));
    }
}

BENCHMARK_TEMPLATE(BM, encrypt)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, encrypt_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();