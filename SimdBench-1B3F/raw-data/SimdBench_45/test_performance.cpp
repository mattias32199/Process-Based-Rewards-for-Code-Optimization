template<void (*Func)(const float*, const float*, const float*, const float*,
    const bool*, const bool*, float*,
    size_t, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t dim1 = state.range(0);
    size_t dim2 = state.range(1);
    size_t dim3 = state.range(2);
    size_t total = dim1 * dim2 * dim3;

    std::vector<float> A(total); rng.initialize_vector_with_random_values(A);
    std::vector<float> B(total); rng.initialize_vector_with_random_values(B);
    std::vector<float> C(total); rng.initialize_vector_with_random_values(C);
    std::vector<float> D(total); rng.initialize_vector_with_random_values(D);

    std::vector<uint8_t> cond1_data(total);
    rng.initialize_vector_with_random_values(cond1_data, true);
    const bool* cond1 = reinterpret_cast<const bool*>(cond1_data.data());
    
    std::vector<uint8_t> cond2_data(total);
    rng.initialize_vector_with_random_values(cond2_data, true);
    const bool* cond2 = reinterpret_cast<const bool*>(cond2_data.data());
    std::vector<float> out(total);

    for (auto _ : state) {
        Func(A.data(), B.data(), C.data(), D.data(),
            cond1, cond2, out.data(),
            dim1, dim2, dim3);
        benchmark::DoNotOptimize(out.data());
    }
}

BENCHMARK_TEMPLATE(BM, tensor_multi_select)->Name("Scalar")->Large_Args_3D;
BENCHMARK_TEMPLATE(BM, tensor_multi_select_simd)->Name("SIMD")->Large_Args_3D;

BENCHMARK_MAIN();