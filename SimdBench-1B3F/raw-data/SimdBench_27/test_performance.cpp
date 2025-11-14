template<void (*Func)(const double*, const double*, double*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t m = state.range(0);
    size_t n = state.range(1);
    
    std::vector<double> mat1(m * n);
    rng.initialize_vector_with_random_values(mat1);
    std::vector<double> mat2(m * n);
    rng.initialize_vector_with_random_values(mat2);
    std::vector<double> dst(m * n);

    for (auto _ : state) {
        Func(mat1.data(), mat2.data(), dst.data(), m, n);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, matrix_hadamard_product)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_hadamard_product_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();