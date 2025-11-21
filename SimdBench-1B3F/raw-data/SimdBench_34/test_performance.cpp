
template<void (*Func)(size_t, const int64_t, const int64_t*, int64_t *)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);

    int64_t a = rng.randint<int64_t>( - (1 << 20), (1 << 20));
    std::vector<int64_t> x(length); 
    for (auto & item: x) { item = rng.randint<int64_t>( - (1 << 20), (1 << 20)); }
    std::vector<int64_t> y(length);
    for(int i=0; i<length; ++i){
        int64_t item = rng.randint<int64_t>( - (1 << 20), (1 << 20));
        y[i] = item;
    }

    for (auto _ : state) {
        Func(length, a, x.data(), y.data());
        benchmark::DoNotOptimize(y);
    }
}

BENCHMARK_TEMPLATE(BM, axm_abs)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, axm_abs_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();