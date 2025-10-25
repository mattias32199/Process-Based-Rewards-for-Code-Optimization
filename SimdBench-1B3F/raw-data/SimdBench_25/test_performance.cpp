template<void (*Func)(const float*, float*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    
    std::vector<float> src(rows * cols);
    rng.initialize_vector_with_random_values(src);
    std::vector<float> dst(rows * cols);

    for (auto _ : state) {
        Func(src.data(), dst.data(), rows, cols);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, matrix_transpose_round_quarter)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_transpose_round_quarter_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();