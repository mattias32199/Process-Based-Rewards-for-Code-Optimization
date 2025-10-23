template<void (*Func)(const int16_t*, int32_t*, size_t)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int16_t> A(length);
    std::vector<int32_t> B(length);
    
    rng.initialize_vector_with_random_values(A);

    for (auto _ : state) {
        Func(A.data(), B.data(), length);
        benchmark::DoNotOptimize(B.data());
    }
}

BENCHMARK_TEMPLATE(BM, int16_to_int32)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, int16_to_int32_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();