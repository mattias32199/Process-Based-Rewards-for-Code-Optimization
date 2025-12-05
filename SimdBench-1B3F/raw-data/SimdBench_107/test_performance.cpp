template<bool (*Func)(std::map<std::string, std::string>)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    const std::string charset = CHARSET_UPPERCASE;
    std::map<std::string, std::string> dict;
    for (size_t j = 0; j < length; j++) {
        std::string key = rng.random_string(charset, rng.randint<size_t>(1, 20));
        std::string value = rng.random_string(charset, rng.randint<size_t>(1, 20));
        dict[key] = value;
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(dict));
    }
}

BENCHMARK_TEMPLATE(BM, check_dict_case)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, check_dict_case_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();