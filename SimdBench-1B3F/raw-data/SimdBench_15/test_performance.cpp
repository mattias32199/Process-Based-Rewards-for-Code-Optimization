template<void (*Func)(const uint16_t*, const uint16_t*, uint32_t, uint16_t*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    uint32_t mask = rng.randint<uint32_t>();
    std::vector<uint16_t> src1(length); rng.initialize_vector_with_random_values(src1);
    std::vector<uint16_t> src2(length); rng.initialize_vector_with_random_values(src2);
    std::vector<uint16_t> dst(length);

    for (auto _ : state) {
        Func(src1.data(), src2.data(), mask, dst.data(), length);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, vector_blend)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, vector_blend_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();