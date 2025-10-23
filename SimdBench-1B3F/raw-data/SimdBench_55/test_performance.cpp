template<void (*Func)(const float*, const int32_t*, float*, size_t, float, float)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<float> A(length);
    std::vector<int32_t> control(length);
    std::vector<float> B(length);
    std::vector<float> B_simd(length);
    
    rng.initialize_vector_with_random_values(A);
    rng.initialize_vector_with_random_values(control);

    float a = rng.uniform<float>(), b = rng.uniform<float>();
    float min_val = std::min(a, b);
    float max_val = std::max(a, b) + 0.1; // assert(min_val != max_val)

    for (auto _ : state) {
        Func(A.data(), control.data(), B.data(), length, min_val, max_val);
        benchmark::DoNotOptimize(B.data());
    }
}

BENCHMARK_TEMPLATE(BM, conditional_normalize)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, conditional_normalize_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();