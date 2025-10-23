template<void (*Func)(const int32_t*, const bool*, float*, size_t)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int32_t> A(length);
    std::vector<u_int8_t> cond(length);
    std::vector<float> diff(length);
    
    rng.initialize_vector_with_random_values(A);
    rng.initialize_vector_with_random_values(cond, true);

    for (auto _ : state) {
        Func(A.data(), reinterpret_cast<const bool*>(cond.data()), diff.data(), length);
        benchmark::DoNotOptimize(diff.data());
    }
}

BENCHMARK_TEMPLATE(BM, conditional_diff)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, conditional_diff_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();