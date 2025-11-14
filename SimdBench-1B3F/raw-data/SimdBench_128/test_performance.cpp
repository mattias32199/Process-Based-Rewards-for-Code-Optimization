template<std::vector<int> (*Func)(const std::vector<int>&)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> lst(length);
    rng.initialize_vector_with_random_values(lst);
    
    // Ensure we have both positive and negative numbers
    if (length > 1) {
        lst[0] = -abs(lst[0]);
        lst[1] = abs(lst[1]);
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(lst));
    }
}

BENCHMARK_TEMPLATE(BM, largest_smallest_integers)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, largest_smallest_integers_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();