template<int64_t (*Func)(const std::vector<int64_t>&)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int64_t> nums(length);
    for(auto& number: nums){
        number = rng.randint<int64_t>(-1024, 1024);
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(nums));
    }
}

BENCHMARK_TEMPLATE(BM, minSubArraySum)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, minSubArraySum_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();