
template<std::vector<int> (*Func)(std::vector<int>)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> numbers(length);
    rng.initialize_vector_with_random_values(numbers);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(numbers));
    }
}

BENCHMARK_TEMPLATE(BM, incr_list)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, incr_list_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();