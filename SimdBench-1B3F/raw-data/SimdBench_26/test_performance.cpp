template<void (*Func)(const float*, const float*, float*, size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t dim1 = state.range(0);
    size_t dim2 = state.range(1);
    size_t dim3 = state.range(2);
    
    std::vector<float> tensor1(dim1 * dim2 * dim3); rng.initialize_vector_with_random_values(tensor1);
    std::vector<float> tensor2(dim1 * dim2 * dim3); rng.initialize_vector_with_random_values(tensor2);
    std::vector<float> dst(dim1 * dim2 * dim3);

    for (auto _ : state) {
        Func(tensor1.data(), tensor2.data(), dst.data(), dim1, dim2, dim3);
        benchmark::DoNotOptimize(dst.data());
    }
}

BENCHMARK_TEMPLATE(BM, tensor3d_sub_round_zero)->Name("Scalar")->Large_Args_3D;
BENCHMARK_TEMPLATE(BM, tensor3d_sub_round_zero_simd)->Name("SIMD")->Large_Args_3D;

BENCHMARK_MAIN();