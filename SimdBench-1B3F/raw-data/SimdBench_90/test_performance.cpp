template<bool (*Func)(std::string)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::string brackets;
    if(rng.random_bool()){
        for (int i = 0; i < length; ++i) {
            brackets += "()";
        }
    }else{
        brackets = std::string(length, '(') + std::string(length, ')');
    }

    for (auto _ : state) {
        bool res = Func(brackets);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, correct_bracketing)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, correct_bracketing_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();