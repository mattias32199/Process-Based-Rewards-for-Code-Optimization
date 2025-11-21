template<void (*Func)(const double*, float*, size_t)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<double> A(length);
    std::vector<float> B(length);
    
    for(auto & item: A) { item = (double)(FLT_MAX) * rng.normal_distribution<double>(0.0, 2.0); }

    for (auto _ : state) {
        Func(A.data(), B.data(), length);
        benchmark::DoNotOptimize(B.data());
    }
}

BENCHMARK_TEMPLATE(BM, double_to_float_saturate)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, double_to_float_saturate_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();