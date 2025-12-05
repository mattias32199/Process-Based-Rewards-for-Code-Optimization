template<bool (*Func)(int64_t)>
static void BM(benchmark::State& state) {
    Random rng(state.range(0));
    std::vector<int64_t> large_primes = {100000007, 1000000007, 10000000019, 100000000003, 1000000000039,
    100000841, 1000000931, 10000001051, 10000000949, 100000000747, 1000000001123};
    
    int64_t n = rng.choice(large_primes);
    for (auto _ : state) {
        bool res = Func(n);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, is_prime)->Name("Scalar")->RangeMultiplier(2)->Range(2, 2<<8);
BENCHMARK_TEMPLATE(BM, is_prime_simd)->Name("SIMD")->RangeMultiplier(2)->Range(2, 2<<8);

BENCHMARK_MAIN();