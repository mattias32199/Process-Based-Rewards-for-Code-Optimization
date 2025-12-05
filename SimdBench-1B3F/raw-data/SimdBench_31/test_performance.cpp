template<void (*Func)(const int16_t*, int16_t*, uint8_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);

    std::vector<int16_t> src(length); rng.initialize_vector_with_random_values(src);
    std::vector<int16_t> dst(length);
    uint8_t base = rng.randint<uint8_t>(1, 255);

    for (auto _ : state) {
        Func(src.data(), dst.data(), base, length);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, nearest_multiple)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, nearest_multiple_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();