// Benchmarking function
template<void (*Func)(const uint8_t *, uint8_t *, size_t, size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t dim1 = state.range(0);
    size_t dim2 = state.range(1);
    size_t dim3 = state.range(2);
    size_t slice_idx = rng.randint(size_t(0), dim3 - 1);
    
    std::vector<uint8_t> tensor(dim1 * dim2 * dim3); rng.initialize_vector_with_random_values(tensor);
    std::vector<uint8_t> slice(dim1 * dim2); rng.initialize_vector_with_random_values(slice);
    std::vector<uint8_t> slice_simd(slice);
    
    for (auto _ : state) {
        Func(tensor.data(), slice.data(), dim1, dim2, dim3, slice_idx);
        benchmark::DoNotOptimize(slice.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, extract_tensor_slice)->Name("Scalar")->Large_Args_3D;
BENCHMARK_TEMPLATE(BM, extract_tensor_slice_simd)->Name("SIMD")->Large_Args_3D;

BENCHMARK_MAIN();
