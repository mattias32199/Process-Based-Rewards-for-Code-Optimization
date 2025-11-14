template<std::string (*Func)(const std::vector<int>&, const std::vector<int>&)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> lst1(length);
    std::vector<int> lst2(length);
    rng.initialize_vector_with_random_values(lst1);
    rng.initialize_vector_with_random_values(lst2);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(lst1, lst2));
    }
}

BENCHMARK_TEMPLATE(BM, myexchange)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, myexchange_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();