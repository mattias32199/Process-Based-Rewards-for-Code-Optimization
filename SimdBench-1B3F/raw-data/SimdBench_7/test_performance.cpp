
// Benchmarking function
template<float (*Func)(const float *, size_t)>
static void BM(benchmark::State& state) {
    Random rng;
    size_t length = state.range(0);
    std::vector<float> array(length*length*length); rng.initialize_vector_with_random_values(array);
    std::vector<float> array_simd(array);
    for (auto _ : state) {
        Func(array.data(), length);
        benchmark::DoNotOptimize(array.data()); // Prevent optimization of the result
    }
}

BENCHMARK_TEMPLATE(BM, diagonal_sum_3d)->Name("Scalar")->RangeMultiplier(2)->Range(1 << 3, 1 << 7);
BENCHMARK_TEMPLATE(BM, diagonal_sum_3d_simd)->Name("SIMD")->RangeMultiplier(2)->Range(1 << 3, 1 << 7);

BENCHMARK_MAIN();
