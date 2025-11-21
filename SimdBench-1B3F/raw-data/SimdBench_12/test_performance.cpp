template<void (*Func)(const int*, int*, uint8_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    uint8_t shift = rng.randint<uint8_t>(1, 31);
    std::vector<int> src(length); 
    rng.initialize_vector_with_random_values(src);
    std::vector<int> dst(length);

    for (auto _ : state) {
        Func(src.data(), dst.data(), shift, length);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, mixed_right_shift)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, mixed_right_shift_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();