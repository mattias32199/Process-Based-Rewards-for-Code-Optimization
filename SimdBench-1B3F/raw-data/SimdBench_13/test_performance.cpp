
template<int64_t (*Func)(const int*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    std::vector<int> matrix(rows * cols);
    rng.initialize_vector_with_random_values(matrix);

    for (auto _ : state) {
        int result = Func(matrix.data(), rows, cols);
        benchmark::DoNotOptimize(&result);
    }
}

BENCHMARK_TEMPLATE(BM, matrix_sum)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_sum_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();