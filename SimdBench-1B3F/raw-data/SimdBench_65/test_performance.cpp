template<std::vector<int> (*Func)(std::vector<int>, int)>
static void BM(benchmark::State& state) {
    Random rng;

    size_t length = state.range(0);
    std::vector<int> numbers(length);
    rng.initialize_vector_with_random_values(numbers);
    int delimeter = rng.randint<int>();

    for (auto _ : state) {
        std::vector<int> res = Func(numbers, delimeter);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, intersperse)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, intersperse_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();