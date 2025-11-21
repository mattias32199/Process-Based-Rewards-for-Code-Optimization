template<void (*Func)(float*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t m = state.range(0);
    size_t n = state.range(1);
    
    std::vector<float> mat(m * n);
    rng.initialize_vector_with_random_values(mat);

    for (auto _ : state) {
        Func(mat.data(), m, n);
        benchmark::DoNotOptimize(mat.data());
    }
}

BENCHMARK_TEMPLATE(BM, matrix_normalize_rows)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_normalize_rows_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();