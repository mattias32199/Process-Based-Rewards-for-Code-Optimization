template<std::string (*Func)(const std::string &)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    size_t length = state.range(0);
    
    std::string charset;
    if(rng.random_bool()) charset = CHARSET_PRINTABLE;
    else charset = CHARSET_DIGITS;
    
    std::string s = rng.random_string(charset, length);

    for (auto _ : state) {
        std::string res = Func(s);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, solve)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, solve_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();