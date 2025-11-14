
// Benchmarking function
template<void (*Func)(const float *, const int *, float *, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<float> src(length); rng.initialize_vector_with_random_values(src);
    std::vector<int> indices(length); for(auto& index : indices){ index = rng.randint(size_t(0), length - 1); }
    std::vector<float> dst(length); rng.initialize_vector_with_random_values(dst);
    
    for (auto _ : state) {
        Func(src.data(), indices.data(), dst.data(), length);
        benchmark::DoNotOptimize(dst.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, indexed_access)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, indexed_access_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();
