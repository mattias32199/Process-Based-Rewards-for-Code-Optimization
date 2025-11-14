template<std::vector<std::string> (*Func)(const std::vector<std::string> &)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    size_t length = state.range(0);
    const std::string charset = CHARSET_DIGITS;

    std::vector<std::string> lst;
    for(int i=0; i<length; ++i){
        std::string s = rng.random_string(charset, rng.randint<int>(0, 20));
        lst.push_back(s);
    }

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(lst));
    }
}

BENCHMARK_TEMPLATE(BM, odd_count)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, odd_count_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();