
template<void (*Func)(const uint32_t*, uint8_t*, size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t dim1 = state.range(0);
    size_t dim2 = state.range(1);
    size_t dim3 = state.range(2);
    size_t total = dim1 * dim2 * dim3;
    
    std::vector<uint32_t> A(total);
    for (auto& val : A) {
        val = rng.randint<uint32_t>();
    }
    
    std::vector<uint8_t> out(total);

    for (auto _ : state) {
        Func(A.data(), out.data(), dim1, dim2, dim3);
        benchmark::DoNotOptimize(out.data());
    }
}

BENCHMARK_TEMPLATE(BM, tensor_bit_count)->Name("Scalar")->Large_Args_3D;
BENCHMARK_TEMPLATE(BM, tensor_bit_count_simd)->Name("SIMD")->Large_Args_3D;

BENCHMARK_MAIN();