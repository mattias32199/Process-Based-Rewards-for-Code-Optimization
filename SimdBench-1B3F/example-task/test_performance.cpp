
template<void (*Func)(const float*, const float*, float*, size_t)>
static void BM_Example(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<float> a(length); rng.initialize_vector_with_random_values(a);
    std::vector<float> b(length); rng.initialize_vector_with_random_values(b);
    std::vector<float> c(length); rng.initialize_vector_with_random_values(c);
    
    for (auto _ : state) {
        Func(a.data(), b.data(), c.data(), length);
        benchmark::DoNotOptimize(c.data()); // Prevent optimization of the result
    }
}

// Register benchmarks
BENCHMARK_TEMPLATE(BM_Example, add_float)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM_Example, add_float_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();
