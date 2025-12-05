template<std::vector<int64_t> (*Func)(std::vector<int16_t>)>
static void BM(benchmark::State& state) {
    Random rng;

    size_t length = state.range(0);
    std::vector<int16_t> numbers(length);
    for(auto& item:numbers){
        item = 1;
    }

    for (auto _ : state) {
        std::vector<int64_t> res = Func(numbers);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, sum_product)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, sum_product_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();
