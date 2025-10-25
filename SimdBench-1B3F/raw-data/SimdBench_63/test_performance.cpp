template<bool (*Func)(std::vector<int>)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);

    size_t length = state.range(0);
    std::vector<int> operations(length);
    for(auto& item:operations){
        item = rng.randint<int>(-10.0, 20.0);
    }

    for (auto _ : state) {
        bool res = Func(operations);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, below_zero)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, below_zero_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();