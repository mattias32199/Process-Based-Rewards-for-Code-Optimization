template<bool (*Func)(std::string)>
static void BM(benchmark::State& state) {
    Random rng;
    std::vector<std::string> charsets = {
        CHARSET_LOWERCASE, CHARSET_UPPERCASE, CHARSET_DIGITS, CHARSET_ALPHA,
        CHARSET_ALPHANUMERIC, CHARSET_HEX, CHARSET_PRINTABLE
    };
    const size_t length = state.range(0);  
    std::string charset = rng.choice(charsets);
    std::string text = rng.random_string(charset, length);  
    for(int ii = 0; ii < text.size() / 2; ii++) 
        text[ii] = text[text.size() - 1 - ii];
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(text));
    }
}

BENCHMARK_TEMPLATE(BM, is_palindrome)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, is_palindrome_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();