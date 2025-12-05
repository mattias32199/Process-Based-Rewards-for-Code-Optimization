template<int (*Func)(const std::string &)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    const std::string charset = CHARSET_HEX;
    std::string num = rng.random_string(charset, length);

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(num));
    }
}

BENCHMARK_TEMPLATE(BM, hex_key)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, hex_key_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();