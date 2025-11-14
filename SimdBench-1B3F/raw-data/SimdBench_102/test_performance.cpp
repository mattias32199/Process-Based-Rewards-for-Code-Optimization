template<bool (*Func)(const std::string &)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    std::vector<size_t> large_primes = {8503057, 9834497, 29986577, 40960001, 45212177, 59969537, 65610001};
    const std::string charset = CHARSET_ALPHA;
    size_t length = rng.choice(large_primes);
    std::string s = "";
    for(int i=0; i<length; ++i) s.push_back(charset[i%charset.length()]);

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(s));
    }
}

BENCHMARK_TEMPLATE(BM, prime_length)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, prime_length_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();