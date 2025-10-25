template<void (*Func)(const float*, int32_t*, size_t)>
static void BM_FloatToIntSaturate(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<float> src(length); 
    for (auto & element: src) { element = ((int64_t)1 << 32) * rng.normal_distribution<float>(0.0, 1.0); }
    std::vector<int32_t> dst(length); 
    rng.initialize_vector_with_random_values(dst);
    
    for (auto _ : state) {
        Func(src.data(), dst.data(), length);
        benchmark::DoNotOptimize(dst.data());
    }
}


BENCHMARK_TEMPLATE(BM_FloatToIntSaturate, float_to_int_saturate)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM_FloatToIntSaturate, float_to_int_saturate_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();