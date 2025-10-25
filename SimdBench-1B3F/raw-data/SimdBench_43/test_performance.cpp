template<void (*Func)(const int8_t*, const int8_t*, int8_t*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t total = state.range(0);
    
    std::vector<int8_t> A(total); rng.initialize_vector_with_random_values(A);      
    std::vector<int8_t> B(total); rng.initialize_vector_with_random_values(B); 
    std::vector<int8_t> C(total);

    for (auto _ : state) {
        Func(A.data(), B.data(), C.data(), total);
        benchmark::DoNotOptimize(C.data());
    }
}

BENCHMARK_TEMPLATE(BM, tensor_three_valued_logic)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, tensor_three_valued_logic_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();