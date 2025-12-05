template<std::vector<int> (*Func)(int)>
static void BM(benchmark::State& state) {
    Random rng(state.range(0));
    int n = rng.randint<int>(1 << 15, 1 << 30);

    for (auto _ : state) {
        std::vector<int> res = Func(n);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, factorize)->Name("Scalar")->RangeMultiplier(2)->Range(2, 2<<8);
BENCHMARK_TEMPLATE(BM, factorize_simd)->Name("SIMD")->RangeMultiplier(2)->Range(2, 2<<8);

BENCHMARK_MAIN();