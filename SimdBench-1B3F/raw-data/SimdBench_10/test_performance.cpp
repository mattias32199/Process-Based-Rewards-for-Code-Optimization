template<void (*Func)(const int64_t*, const int64_t*, int64_t*, int64_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<int64_t> src1(length); for ( auto & element : src1) { element = rng.randint<int64_t>(- (1 << 28), 1 << 28); }
    std::vector<int64_t> src2(length); for ( auto & element : src2) { element = rng.randint<int64_t>(- (1 << 28), 1 << 28); }
    std::vector<int64_t> dst(length); 
    int64_t scalar = rng.randint<int64_t>(- (1 << 28), 1 << 28);

    for (auto _ : state) {
        Func(src1.data(), src2.data(), dst.data(), scalar, length);
        benchmark::DoNotOptimize(dst.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, vector_vector_add)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, vector_vector_add_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();