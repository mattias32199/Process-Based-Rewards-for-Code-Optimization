template<std::string (*Func)(std::string, std::string)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    const std::string charset = "01";
    std::string a = rng.random_string(charset, length);
    std::string b = rng.random_string(charset, length);

    for (auto _ : state) {
        std::string res = Func(a, b);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, string_xor)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, string_xor_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();