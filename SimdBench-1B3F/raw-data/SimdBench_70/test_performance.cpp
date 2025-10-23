template<uint64_t (*Func)(uint64_t, uint64_t)>
static void BM(benchmark::State& state) {
    Random rng(state.range(0));
    uint64_t a = rng.randint<uint64_t>(1);
    uint64_t b = rng.randint<uint64_t>(1, 1024);

    for (auto _ : state) {
        uint64_t res = Func(a, b);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, greatest_common_divisor)->Name("Scalar")->RangeMultiplier(2)->Range(2, 2<<8);
BENCHMARK_TEMPLATE(BM, greatest_common_divisor_simd)->Name("SIMD")->RangeMultiplier(2)->Range(2, 2<<8);

BENCHMARK_MAIN();