template<std::vector<float> (*Func)(const std::vector<float> &)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<float> numbers(length);
    rng.initialize_vector_with_random_values(numbers);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(numbers));
    }
}

BENCHMARK_TEMPLATE(BM, get_positive)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, get_positive_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();