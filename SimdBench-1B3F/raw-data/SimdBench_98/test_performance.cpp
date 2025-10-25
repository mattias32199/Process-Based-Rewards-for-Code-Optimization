template<std::vector<std::string> (*Func)(const std::vector<std::string> &, const std::vector<std::string> &)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0)/4;
    const std::string charset = CHARSET_PRINTABLE;
    std::vector<std::string> lst1(length);
    std::vector<std::string> lst2(length);

    for(auto & s: lst1) s = rng.random_string(charset, rng.randint(0,10));
    for(auto & s: lst2) s = rng.random_string(charset, rng.randint(0,20));

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(lst1, lst2));
    }
}

BENCHMARK_TEMPLATE(BM, total_match)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, total_match_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();