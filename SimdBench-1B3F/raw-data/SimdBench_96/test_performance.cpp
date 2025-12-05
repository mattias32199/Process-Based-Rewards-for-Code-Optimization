template<bool (*Func)(std::vector<int>, int)>
static void BM(benchmark::State& state) {
    const size_t length = state.range(0);
    Random rng;
    std::vector<int> numbers(length);
    for(auto & element : numbers) {
        element = rng.randint<int>(-50, 100);
    }
    for(int j = 0; j < length / 2; j++) {
        numbers[length - 1 - j] = numbers[j];
    }
    int weight = rng.randint<int>(0, 10000);
    
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(numbers, weight));
    }
}

BENCHMARK_TEMPLATE(BM, will_it_fly)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, will_it_fly_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();