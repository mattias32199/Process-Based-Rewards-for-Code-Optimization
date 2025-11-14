template<void (*Func)(const int32_t*, int8_t, float*, size_t)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int32_t> src(length); rng.initialize_vector_with_random_values(src);
    std::vector<float> dst(length);
    const int8_t scale = rng.randint<int8_t>();
    
    for (auto _ : state) {
        Func(src.data(), scale, dst.data(), length);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, int_to_float_scaled)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, int_to_float_scaled_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();