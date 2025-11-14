template<void (*Func)(const float*, const float*, int8_t*, float, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t total = state.range(0);
    float epsilon = rng.normal_distribution<float>(0.0f, 10.0f);

    std::vector<float> A(total); rng.initialize_vector_with_random_values(A);
    std::vector<float> B(total); rng.initialize_vector_with_random_values(B);
    std::vector<int8_t> out(total);

    for (auto _ : state) {
        Func(A.data(), B.data(), out.data(), epsilon, total);
        benchmark::DoNotOptimize(out.data());
    }
}

BENCHMARK_TEMPLATE(BM, tensor_ternary_compare)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, tensor_ternary_compare_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();