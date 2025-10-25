template<void (*Func)(const double*, double, double*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t m = state.range(0);
    size_t n = state.range(1);
    double scalar = rng.uniform<double>();
    std::vector<double> mat(m * n);
    rng.initialize_vector_with_random_values(mat);
    std::vector<double> dst(m * n);

    for (auto _ : state) {
        Func(mat.data(), scalar, dst.data(), m, n);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, matrix_scalar_mul)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_scalar_mul_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();