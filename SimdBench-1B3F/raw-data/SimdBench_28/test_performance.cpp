template<void (*Func)(const float*, const float*, float*, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t m = state.range(0);
    size_t n = state.range(1);
    
    std::vector<float> mat(m * n); rng.initialize_vector_with_random_values(mat);
    std::vector<float> vec(n); rng.initialize_vector_with_random_values(vec);
    std::vector<float> dst(m);

    for (auto _ : state) {
        Func(mat.data(), vec.data(), dst.data(), m, n);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, matrix_vector_mul)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, matrix_vector_mul_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();