template<void (*Func)(const int*, int*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    size_t total = rows * cols;
    
    std::vector<int> A(total); rng.initialize_vector_with_random_values(A);
    std::vector<int> B(total);

    for (auto _ : state) {
        Func(A.data(), B.data(), rows, cols);
        benchmark::DoNotOptimize(B.data());
    }
}

BENCHMARK_TEMPLATE(BM, matrix_conditional_abs)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_conditional_abs_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();