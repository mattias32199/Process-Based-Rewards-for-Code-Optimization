
// Benchmarking function
template<void (*Func)(const double*, double*, size_t, size_t, const size_t*)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    std::vector<size_t> indices(rows);
    for (auto & index : indices) {
        index = rng.randint(size_t(0), rows - 1);
    }
    std::vector<double> src(rows * cols); rng.initialize_vector_with_random_values(src);
    std::vector<double> dst(rows * cols); rng.initialize_vector_with_random_values(dst);

    for (auto _ : state) {
        Func(src.data(), dst.data(), rows, cols, indices.data());
        benchmark::DoNotOptimize(dst.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, reorder_matrix_rows)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, reorder_matrix_rows_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();
