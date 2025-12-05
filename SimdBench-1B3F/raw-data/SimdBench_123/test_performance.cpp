template<int (*Func)(const std::vector<int> &)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> arr(length);
    std::vector<int> choiceset = {-2, -1, 0, 1, 2};
    for(auto& num: arr){
        num = rng.choice(choiceset);
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(arr));
    }
}

BENCHMARK_TEMPLATE(BM, prod_signs)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, prod_signs_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();