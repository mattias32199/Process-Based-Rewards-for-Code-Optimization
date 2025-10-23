template<bool (*Func)(const int*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    
    std::vector<int> matrix(rows * cols);
    
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            matrix[r * cols + c] = static_cast<int>(r * cols + c);
        }
    }

    bool wrong_case = rng.random_bool();
    if (wrong_case && rows > 0 && cols > 0) {
        size_t r = rng.randint(size_t(0), rows-1);
        size_t c = rng.randint(size_t(0), cols-1);
        matrix[r*cols + c] = rng.randint<int>();
    }

    for (auto _ : state) {
        bool result = Func(matrix.data(), rows, cols);
        benchmark::DoNotOptimize(&result);
    }
}

BENCHMARK_TEMPLATE(BM, matrix_rows_strictly_increasing)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_rows_strictly_increasing_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();