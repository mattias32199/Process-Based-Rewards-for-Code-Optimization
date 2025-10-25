template<void (*Func)(const double*, const double*, double*, size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t m = state.range(0);
    size_t n = state.range(1);
    size_t p = state.range(2);
    
    std::vector<double> mat1(m * n); rng.initialize_vector_with_random_values(mat1);
    std::vector<double> mat2(n * p); rng.initialize_vector_with_random_values(mat2);
    std::vector<double> dst(m * p);

    for (auto _ : state) {
        Func(mat1.data(), mat2.data(), dst.data(), m, n, p);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, matrix_mul_round_int)->Name("Scalar")->Large_Args_3D;
BENCHMARK_TEMPLATE(BM, matrix_mul_round_int_simd)->Name("SIMD")->Large_Args_3D;

BENCHMARK_MAIN();