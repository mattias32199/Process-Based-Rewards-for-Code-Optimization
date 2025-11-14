template<void (*Func)(const uint32_t*, uint64_t*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    const size_t length = state.range(0);

    std::vector<uint32_t> src(length);
    std::vector<uint64_t> dst(length);
    rng.initialize_vector_with_random_values(src);

    for (auto _ : state) {
        Func(src.data(), dst.data(), length);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, widening_uint)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, widening_uint_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();