bool correctness_check(int iterations) {
    Random rng;
    for (int  i = 0; i < iterations; i++){
        /* one check */
        int length = Small_Arg_1D;
        std::vector<int16_t> arr(length); rng.initialize_vector_with_random_values(arr);
        std::vector<int16_t> arr_simd(arr);
        uint64_t start = rng.randint<uint64_t>(0, length - 1);
        uint64_t end = rng.randint<uint64_t>(start, length - 1);
        range_reverse(arr.data(), start, end);
        range_reverse_simd(arr_simd.data(), start, end);
        
        if(!allclose(arr, arr_simd)){
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