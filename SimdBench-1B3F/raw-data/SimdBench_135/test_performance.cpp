template<std::vector<int> (*Func)(const std::vector<int>&, const std::vector<int>&)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> game(length);
    std::vector<int> guess(length);
    for( int i=0; i< length; ++i){
        game[i] = rng.randint<int>(-1000, 1000);
        guess[i] = rng.randint<int>(-1000, 1000);
    }
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(game, guess));
    }
}

BENCHMARK_TEMPLATE(BM, compare)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, compare_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();