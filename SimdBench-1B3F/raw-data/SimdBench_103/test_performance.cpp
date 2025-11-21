template<std::string (*Func)(uint64_t)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    const uint64_t N = rng.randint<uint64_t>();

    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(N));
    }
}

BENCHMARK_TEMPLATE(BM, solve)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, solve_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();