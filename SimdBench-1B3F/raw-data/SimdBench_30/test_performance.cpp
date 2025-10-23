template<bool (*Func)(const int*, const bool*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    
    std::vector<int> matrix(rows * cols);
    rng.initialize_vector_with_random_values(matrix);
    std::vector<uint8_t> directions(rows);
    rng.initialize_vector_with_random_values(directions, true);

    // Sort the matrix rows according to directions
    for (size_t i = 0; i < rows; i++) {
        if (directions[i]) {
            std::sort(matrix.begin() + i*cols, matrix.begin() + (i+1)*cols);
        } else {
            std::sort(matrix.begin() + i*cols, matrix.begin() + (i+1)*cols, std::greater<int>());
        }
    }

    for (auto _ : state) {
        bool result = Func(matrix.data(), reinterpret_cast<const bool*>(directions.data()), rows, cols);
        benchmark::DoNotOptimize(&result);
    }
}

BENCHMARK_TEMPLATE(BM, matrix_rows_sorted_verify)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_rows_sorted_verify_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();