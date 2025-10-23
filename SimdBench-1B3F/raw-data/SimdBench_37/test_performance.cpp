template<bool (*Func)(const int*, size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    size_t block_size = rng.randint(size_t(1), size_t(8));
    size_t rows = block_size * static_cast<size_t>(state.range(0));
    size_t cols = block_size * static_cast<size_t>(state.range(1));
    
    std::vector<int> matrix(rows * cols);
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            matrix[r * cols + c] = static_cast<int>((r % block_size) + (c % block_size));
        }
    }

    if (rng.random_bool()) {
        size_t r = rng.randint(size_t(0), rows - 1);
        size_t c = rng.randint(size_t(0), cols - 1);
        matrix[r * cols + c] = rng.randint<int>();
    }

    for (auto _ : state) {
        bool result = Func(matrix.data(), block_size, rows, cols);
        benchmark::DoNotOptimize(&result);
    }
}

BENCHMARK_TEMPLATE(BM, matrix_blocks_equal)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_blocks_equal_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();