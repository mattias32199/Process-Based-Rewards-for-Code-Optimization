template<uint64_t (*Func)(const std::vector<float> &)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<float> lst(length);
    
    for (auto& num : lst) {
        num = rng.uniform<float>(-100.0f, 100.0f);
        if (rng.randint<int>() % 4 == 0) {
            num = round(num);
        }
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(lst));
    }
}

BENCHMARK_TEMPLATE(BM, double_the_difference)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, double_the_difference_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();