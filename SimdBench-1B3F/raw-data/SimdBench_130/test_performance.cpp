template<int (*Func)(const std::vector<int> &)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> lst(length);
    std::vector<int> choiceset = {-2, -1, 0, 1, 2};
    for(auto& num: lst){
        num = rng.choice(choiceset);
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(lst));
    }
}

BENCHMARK_TEMPLATE(BM, sum_squares)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, sum_squares_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();