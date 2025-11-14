
template<int64_t (*Func)(const int8_t *, const int8_t *, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<int8_t> A(length); rng.initialize_vector_with_random_values(A);
    std::vector<int8_t> B(length); rng.initialize_vector_with_random_values(B);
    
    for (auto _ : state) {
        int64_t diff = Func(A.data(), B.data(), length);
        benchmark::DoNotOptimize(diff);
    }
}

BENCHMARK_TEMPLATE(BM, squarediff)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, squarediff_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();