template<void (*Func)(const uint16_t*, const uint16_t*, uint16_t*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<uint16_t> src1(length); rng.initialize_vector_with_random_values(src1);
    std::vector<uint16_t> src2(length); rng.initialize_vector_with_random_values(src2);
    std::vector<uint16_t> dst(length);

    for (auto _ : state) {
        Func(src1.data(), src2.data(), dst.data(), length);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, saturating_add)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, saturating_add_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();