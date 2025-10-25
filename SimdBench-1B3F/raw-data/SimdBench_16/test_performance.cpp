template<void (*Func)(const uint32_t*, uint32_t*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<uint32_t> src(length);
    rng.initialize_vector_with_random_values(src);
    std::vector<uint32_t> dst(length);

    for (auto _ : state) {
        Func(src.data(), dst.data(), length);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, population_count)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, population_count_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();