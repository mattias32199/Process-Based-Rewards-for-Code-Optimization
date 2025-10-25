template<void (*Func)(const int64_t*, const int64_t*, int64_t*, size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t dim1 = state.range(0);
    size_t dim2 = state.range(1);
    size_t dim3 = state.range(2);
    
    std::vector<int64_t> A(dim1 * dim2 * dim3); for ( auto & element : A) { element = rng.randint<int64_t>(- (1 << 12), 1 << 12); }
    std::vector<int64_t> B(dim1 * dim2 * dim3); for ( auto & element : B) { element = rng.randint<int64_t>(- (1 << 12), 1 << 12); }
    std::vector<int64_t> C(dim1 * dim2 * dim3);

    for (auto _ : state) {
        Func(A.data(), B.data(), C.data(), dim1, dim2, dim3);
        benchmark::DoNotOptimize(C.data());
    }
}

BENCHMARK_TEMPLATE(BM, tensor_add_3d)->Name("Scalar")->Large_Args_3D;
BENCHMARK_TEMPLATE(BM, tensor_add_3d_simd)->Name("SIMD")->Large_Args_3D;

BENCHMARK_MAIN();