template<size_t (*Func)(const uint8_t*, const uint8_t*, const size_t[4])>
static void BM(benchmark::State& state) {
    Random rng;
    size_t dims[4] = {static_cast<size_t>(state.range(0)),
                      static_cast<size_t>(state.range(1)),
                      static_cast<size_t>(state.range(2)),
                      static_cast<size_t>(state.range(3))};
    size_t total = dims[0] * dims[1] * dims[2] * dims[3];
    
    std::vector<uint8_t> A(total); rng.initialize_vector_with_random_values(A);
    std::vector<uint8_t> B(total); rng.initialize_vector_with_random_values(B);

    size_t n = rng.randint(size_t(0), total);
    while(n-- && total > 0){
        size_t idx = rng.randint(size_t(0), total-1);
        B[idx] = A[idx];
    }

    for (auto _ : state) {
        size_t count = Func(A.data(), B.data(), dims);
        benchmark::DoNotOptimize(&count);
    }
}

BENCHMARK_TEMPLATE(BM, tensor_4d_equal_count)->Name("Scalar")->Large_Args_4D;
BENCHMARK_TEMPLATE(BM, tensor_4d_equal_count_simd)->Name("SIMD")->Large_Args_4D;

BENCHMARK_MAIN();