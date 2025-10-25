template<void (*Func)(const uint16_t*, const uint16_t*, uint16_t*, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t total = state.range(0);
    
    std::vector<uint16_t> A(total);
    rng.initialize_vector_with_random_values(A);
    std::vector<uint16_t> B(total);
    rng.initialize_vector_with_random_values(B);
    std::vector<uint16_t> C(total);

    for (auto _ : state) {
        Func(A.data(), B.data(), C.data(), total);
        benchmark::DoNotOptimize(C.data());
    }
}

BENCHMARK_TEMPLATE(BM, tensor_bit_op)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, tensor_bit_op_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();