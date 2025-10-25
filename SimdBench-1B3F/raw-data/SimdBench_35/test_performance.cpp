template<MinMaxPair (*Func)(const int16_t*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<int16_t> src(length); 
    rng.initialize_vector_with_random_values(src);

    for (auto _ : state) {
        MinMaxPair res = Func(src.data(), length);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, min_max_pair)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, min_max_pair_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();