template<void (*Func)(const double*, const double*, double*, size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t m = state.range(0);
    size_t n = state.range(1);
    size_t p = state.range(2);
    
    std::vector<double> A(m * n);
    for(auto & item: A) { item = rng.normal_distribution<double>(); }
    std::vector<double> B(n * p);
    for(auto & item: B) { item = rng.normal_distribution<double>(); }
    std::vector<double> C(m * p);

    for (auto _ : state) {
        Func(A.data(), B.data(), C.data(), m, n, p);
        benchmark::DoNotOptimize(C.data());
    }
}

BENCHMARK_TEMPLATE(BM, range_matrix_mul)->Name("Scalar")->Large_Args_3D;
BENCHMARK_TEMPLATE(BM, range_matrix_mul_simd)->Name("SIMD")->Large_Args_3D;

BENCHMARK_MAIN();