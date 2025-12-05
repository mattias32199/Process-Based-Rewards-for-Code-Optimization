template<int (*Func)(std::vector<int>, int)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    const int k = length/2;
    Random rng;
    std::vector<int> arr(length);
    rng.initialize_vector_with_random_values(arr);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(arr, k));
    }
}

BENCHMARK_TEMPLATE(BM, add_elements)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, add_elements_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();