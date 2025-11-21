template<bool (*Func)(const double*, const double*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    
    std::vector<double> matrix(rows * cols); rng.initialize_vector_with_random_values(matrix);
    std::vector<double> vector(cols); rng.initialize_vector_with_random_values(vector);

    bool correct_case = rng.random_bool();
    if(correct_case){
        size_t idx = rng.randint( size_t(0), rows-1);
        std::copy(vector.begin(), vector.end(), matrix.begin()+idx*cols);
    }

    for (auto _ : state) {
        bool result = Func(matrix.data(), vector.data(), rows, cols);
        benchmark::DoNotOptimize(&result);
    }
}

BENCHMARK_TEMPLATE(BM, matrix_has_row)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_has_row_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();