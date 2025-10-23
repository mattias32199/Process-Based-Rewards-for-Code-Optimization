template<void (*Func)(const uint32_t*, float*, size_t)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<uint32_t> A(length);
    std::vector<float> B(length);
    
    rng.initialize_vector_with_random_values(A);

    for (auto _ : state) {
        Func(A.data(), B.data(), length);
        benchmark::DoNotOptimize(B.data());
    }
}
BENCHMARK_TEMPLATE(BM, int_bits_to_float)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, int_bits_to_float_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();