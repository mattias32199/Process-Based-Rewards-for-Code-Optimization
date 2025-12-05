template<std::string (*Func)(const std::vector<std::string>&)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    const std::string charset = CHARSET_ALPHANUMERIC;
    std::vector<std::string> strings(length);
    for(auto& item: strings){
        item = rng.random_string(charset, rng.randint(size_t(0), size_t(64)));
    }

    for (auto _ : state) {
        std::string res = Func(strings);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, longest)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, longest_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();