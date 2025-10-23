
bool correctness_check(int iterations) {
    Random rng;
    for (int  i = 0; i < iterations; i++){
        /* one check */
        size_t length = Small_Arg_3D;
        std::vector<float> array(length*length*length); rng.initialize_vector_with_random_values(array);
        std::vector<float> array_simd(array);
        float sum = diagonal_sum_3d(array.data(), length);
        float sum_simd = diagonal_sum_3d_simd(array_simd.data(), length);
        if(!allclose(sum, sum_simd)){
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
