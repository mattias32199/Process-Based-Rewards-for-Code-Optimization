template<void (*Func)(const double*, const double*, size_t, size_t, bool*)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    
    std::vector<double> matrix(rows * cols); for(auto & element: matrix) { element = rng.random<double>(); }
    //rng.initialize_vector_with_random_values(matrix);
    std::vector<double> thresholds(rows); for(auto & element: thresholds) { element = rng.normal_distribution(1.0, 0.75); }
    //rng.initialize_vector_with_random_values(thresholds);

    std::vector<uint8_t> result(rows);

    for (auto _ : state) {
        Func(matrix.data(), thresholds.data(), rows, cols, reinterpret_cast<bool*>(result.data()));
        benchmark::DoNotOptimize(result.data());
    }
}

BENCHMARK_TEMPLATE(BM, matrix_row_norm_compare)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_row_norm_compare_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();