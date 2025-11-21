template<std::string (*Func)(std::string)>
static void BM(benchmark::State& state) {
    Random rng;
    std::vector<std::string> charsets = {
        CHARSET_LOWERCASE, CHARSET_UPPERCASE, CHARSET_DIGITS, CHARSET_ALPHA,
        CHARSET_ALPHANUMERIC, CHARSET_HEX, CHARSET_PRINTABLE
    };
    const size_t length = state.range(0);  
    std::string charset = rng.choice(charsets);
    std::string text = rng.random_string(charset, length);  
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(text));
    }
}

BENCHMARK_TEMPLATE(BM, decode_shift)->Name("Scalar")->Medium_Args_1D;
BENCHMARK_TEMPLATE(BM, decode_shift_simd)->Name("SIMD")->Medium_Args_1D;

BENCHMARK_MAIN();