template<int (*Func)(const int*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<int> src(length); 
    rng.initialize_vector_with_random_values(src);

    for (auto _ : state) {
        int result = Func(src.data(), length);
        benchmark::DoNotOptimize(&result);
    }
}

BENCHMARK_TEMPLATE(BM, vector_even_min)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, vector_even_min_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();