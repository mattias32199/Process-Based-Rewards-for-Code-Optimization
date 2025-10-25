bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t length = Small_Arg_1D;
        std::vector<double> vec(length); rng.initialize_vector_with_random_values(vec);
        std::vector<int16_t> index(length);
        for(auto& item: index){
            item = rng.randint<int16_t>( -(2*length), 2*length );
        }

        double res_scalar = indexed_sum(vec.data(), index.data(), length);
        double res_simd = indexed_sum_simd(vec.data(), index.data(), length);

        if(!allclose(res_scalar, res_simd)){
            if(ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    bool pass = correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}