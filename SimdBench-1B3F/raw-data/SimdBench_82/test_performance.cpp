template<bool (*Func)(std::vector<int>)>
static void BM(benchmark::State& state) {
    const size_t length = sqrt(state.range(0) * state.range(1));
    Random rng(DEFAULT_SEED);
    std::vector<int> numbers(length);
    rng.initialize_vector_with_random_values(numbers);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(numbers));
    }
}

BENCHMARK_TEMPLATE(BM, triples_sum_to_zero)->Name("Scalar")->Large_Args_2D;
BENCHMARK_TEMPLATE(BM, triples_sum_to_zero_simd)->Name("SIMD")->Large_Args_2D;

BENCHMARK_MAIN();