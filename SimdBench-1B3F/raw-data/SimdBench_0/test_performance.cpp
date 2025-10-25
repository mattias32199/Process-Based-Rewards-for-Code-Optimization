template<void (*Func)(const int64_t *, int64_t *, const bool *, size_t)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int64_t> src(length); rng.initialize_vector_with_random_values(src);
    std::vector<int64_t> dst(length); rng.initialize_vector_with_random_values(dst);
    std::vector<uint8_t> mask(length); rng.initialize_vector_with_random_values(mask, true);
    
    for (auto _ : state) {
        Func(src.data(), dst.data(), reinterpret_cast<const bool*>(mask.data()), length);
        benchmark::DoNotOptimize(dst.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, conditional_move)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, conditional_move_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();