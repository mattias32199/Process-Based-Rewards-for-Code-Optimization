template<bool (*Func)(const double*, double, size_t, size_t)>
static void BM(benchmark::State& state) {
    Random rng(DEFAULT_SEED);
    const double tolerance = rng.random<double>();
    size_t length = state.range(0);
    size_t block_size = 32;
    std::vector<double> vec(length); for(auto & element: vec) { element = rng.uniform<double>(); }
    
    // true
    if(block_size < length && block_size > 0) { 
        size_t num_blocks = length / block_size;
        for(int block_n=1; block_n < num_blocks; block_n++){
            for(int k=0; k<block_size; ++k){
                vec[block_n*block_size+k] = vec[ k ];
            }
        } 
    }

    for (auto _ : state) {
        bool result = Func(vec.data(), tolerance, length, block_size);
        benchmark::DoNotOptimize(&result);
    }
}

BENCHMARK_TEMPLATE(BM, vector_block_equal)->Name("Scalar")->Large_Args_1D;
BENCHMARK_TEMPLATE(BM, vector_block_equal_simd)->Name("SIMD")->Large_Args_1D;

BENCHMARK_MAIN();