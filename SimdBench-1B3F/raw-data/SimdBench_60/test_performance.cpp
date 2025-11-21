template<double (*Func)(const double*, const int16_t*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    const size_t length = state.range(0);
    std::vector<double> vec(length); rng.initialize_vector_with_random_values(vec);
    std::vector<int16_t> index(length);
    for(auto& item: index){
        item = rng.randint<int16_t>( -(2*length), 2*length );
    }

    for (auto _ : state) {
        double res = Func(vec.data(), index.data(), length);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK_TEMPLATE(BM, indexed_sum)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, indexed_sum_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();