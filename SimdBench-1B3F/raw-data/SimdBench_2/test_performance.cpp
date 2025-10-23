
// Benchmarking function
template<void (*Func)(const double *, double *, size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t rows = state.range(0);
    size_t cols = state.range(1);
    std::vector<double> src(rows * cols); rng.initialize_vector_with_random_values(src);
    std::vector<double> dst(rows); rng.initialize_vector_with_random_values(dst);
    std::vector<double> dst_simd(dst);
    size_t stride = rng.randint(size_t(0), cols - 1);
    
    for (auto _ : state) {
        Func(src.data(), dst.data(), rows, cols, stride);
        benchmark::DoNotOptimize(dst.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, strided_load_store)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, strided_load_store_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();
