
template<void (*Func)(const uint32_t*, uint32_t*, uint8_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    size_t total = rows * cols;

    std::vector<uint32_t> src(total); rng.initialize_vector_with_random_values(src);
    uint8_t rotate_bits = rng.randint<uint8_t>();
    
    std::vector<uint32_t> dst(total);

    for (auto _ : state) {
        Func(src.data(), dst.data(), rotate_bits, total);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, matrix_conditional_bit_rotate)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_conditional_bit_rotate_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();