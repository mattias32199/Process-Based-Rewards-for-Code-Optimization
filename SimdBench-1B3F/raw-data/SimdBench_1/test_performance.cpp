
// Benchmarking function
template<void (*Func)(const int *, int *, int, size_t)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> src(length); for ( auto & element : src) { element = rng.randint(0, 1 << 12); }
    std::vector<int> dst(length); rng.initialize_vector_with_random_values(dst);
    std::vector<int> dst_simd(dst);
    int scale = rng.randint(1, 100);
    
    for (auto _ : state) {
        Func(src.data(), dst.data(), scale, length);
        benchmark::DoNotOptimize(dst.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, load_modify_store)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, load_modify_store_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();
