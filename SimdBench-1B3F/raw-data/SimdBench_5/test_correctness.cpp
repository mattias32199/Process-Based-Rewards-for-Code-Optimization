
bool correctness_check(int iterations) {
    Random rng;
    for (int  i = 0; i < iterations; i++){
        /* one check */
        size_t dim1 = Small_Arg_3D;
        size_t dim2 = Small_Arg_3D;
        size_t dim3 = Small_Arg_3D;
        size_t slice_idx = rng.randint(size_t(0), dim3 - 1);
        
        std::vector<uint8_t> tensor(dim1 * dim2 * dim3); rng.initialize_vector_with_random_values(tensor);
        std::vector<uint8_t> slice(dim1 * dim2); rng.initialize_vector_with_random_values(slice);
        std::vector<uint8_t> slice_simd(slice);

        extract_tensor_slice(tensor.data(), slice.data(), dim1, dim2, dim3, slice_idx);
        extract_tensor_slice_simd(tensor.data(), slice_simd.data(), dim1, dim2, dim3, slice_idx);
        
        if(!allclose(slice, slice_simd)){
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
