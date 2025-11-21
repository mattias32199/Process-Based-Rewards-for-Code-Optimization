bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t dim1 = Small_Arg_3D;
        size_t dim2 = Small_Arg_3D;
        size_t dim3 = Small_Arg_3D;
        size_t total = dim1 * dim2 * dim3;
        
        std::vector<uint32_t> A(total);
        int idx = 0;
        for (auto& val : A) {
            val = rng.randint<uint32_t>();
            // corner cases
            if (idx % 16 == 0) val = 0;
            else if (idx % 16 == 1) val = ~0u;
            else if (idx % 16 == 2) val = 0x55555555;
            else if (idx % 16 == 3) val = 0xAAAAAAAA;
            idx++;
        }
        
        std::vector<uint8_t> out_scalar(total);
        std::vector<uint8_t> out_simd(total);

        tensor_bit_count(A.data(), out_scalar.data(), dim1, dim2, dim3);
        tensor_bit_count_simd(A.data(), out_simd.data(), dim1, dim2, dim3);
        
        if(!allclose(out_scalar, out_simd)){
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