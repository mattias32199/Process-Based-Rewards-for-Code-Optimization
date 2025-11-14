template<void (*Func)(const int16_t *, int16_t*, size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    size_t block_size = rng.randint(size_t(1), std::min(rows, cols));
    std::vector<int16_t> src(rows * cols); rng.initialize_vector_with_random_values(src);
    std::vector<int16_t> dst(rows * cols); rng.initialize_vector_with_random_values(dst);

    for (auto _ : state) {
        Func(src.data(), dst.data(), rows, cols, block_size);
        benchmark::DoNotOptimize(dst.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, blocked_matrix_transpose)->Name("Scalar")->Large_Args_3D;
BENCHMARK_TEMPLATE(BM, blocked_matrix_transpose_simd)->Name("SIMD")->Large_Args_3D;

BENCHMARK_MAIN();

