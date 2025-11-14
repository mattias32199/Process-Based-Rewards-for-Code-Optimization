template<size_t (*Func)(const int8_t*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<int8_t> src(length);
    rng.initialize_vector_with_random_values(src);

    for (auto _ : state) {
        size_t result = Func(src.data(), length);
        benchmark::DoNotOptimize(&result);
    }
}

BENCHMARK_TEMPLATE(BM, argmax)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, argmax_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();