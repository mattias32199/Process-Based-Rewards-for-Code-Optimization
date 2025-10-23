template<std::string (*Func)(int64_t, int8_t)>
static void BM(benchmark::State& state) {
    const size_t x = state.range(0); 
    int base = 3;   
    for (auto _ : state) {
        benchmark::DoNotOptimize(Func(x, base));
    }
}

BENCHMARK_TEMPLATE(BM, change_base)->Name("Scalar")->Arg(1<<30)->Arg(int64_t(1)<<35)->Arg(int64_t(1)<<40)->Arg(int64_t(1)<<45)
->Arg(int64_t(1)<<50)->Arg(int64_t(1)<<55)->Arg(int64_t(1)<<60);
BENCHMARK_TEMPLATE(BM, change_base_simd)->Name("SIMD")->Arg(1<<30)->Arg(int64_t(1)<<35)->Arg(int64_t(1)<<40)->Arg(int64_t(1)<<45)
->Arg(int64_t(1)<<50)->Arg(int64_t(1)<<55)->Arg(int64_t(1)<<60);

BENCHMARK_MAIN();