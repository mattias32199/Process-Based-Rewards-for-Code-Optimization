template<uint64_t (*Func)(uint64_t)>
static void BM(benchmark::State& state) {
    Random rng(state.range(0));
    uint64_t n = rng.randint<uint64_t>(1 << 12, 1 << 24);

    for (auto _ : state) {
        uint64_t res = Func(n);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, largest_divisor)->Name("Scalar")->RangeMultiplier(2)->Range(2, 2<<8);
BENCHMARK_TEMPLATE(BM, largest_divisor_simd)->Name("SIMD")->RangeMultiplier(2)->Range(2, 2<<8);

BENCHMARK_MAIN();