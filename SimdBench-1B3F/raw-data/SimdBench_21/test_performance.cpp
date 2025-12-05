template<void (*Func)(const float*, const float*, float*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<float> src1(length); rng.initialize_vector_with_random_values(src1);
    std::vector<float> src2(length); rng.initialize_vector_with_random_values(src2);
    std::vector<float> dst(length);

    for (auto _ : state) {
        Func(src1.data(), src2.data(), dst.data(), length);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, vector_mul_round_up)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, vector_mul_round_up_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();