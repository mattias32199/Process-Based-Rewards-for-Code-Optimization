template<void (*Func)(const float*, const float*, int32_t*, size_t)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<float> A(length);
    std::vector<float> B(length);
    std::vector<int32_t> C(length);
    
    for (auto & element: A) { element = ((int64_t)1 << 32) * rng.normal_distribution<float>(0.0, 1.0); }
    rng.initialize_vector_with_random_values(B);

    for (auto _ : state) {
        Func(A.data(), B.data(), C.data(), length);
        benchmark::DoNotOptimize(C.data());
    }
}

BENCHMARK_TEMPLATE(BM, float_div_round)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, float_div_round_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();