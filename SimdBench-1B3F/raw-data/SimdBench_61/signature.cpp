/*
Input to this function are:
- A pointer to a non-empty 64-bit floating-point (double) input array (input);
- A pointer to a non-empty 64-bit floating-point (double) kernel array (kernel);
- A pointer to a non-empty 64-bit floating-point (double) destination array (output);
- The size of the input array (input_size);
- The size of the kernel array (kernel_size).

Your goal is to implement a simple 2D Convolution function (conv2d) using ReLU activation.
The input array is in the shape (input_size, input_size).
The kernel array is in the shape (kernel_size, kernel_size). 
The output array is in the shape (input_size - kernel_size + 1, input_size - kernel_size + 1).
All three arrays are flattened into 1D arrays.
The requirement is to implement the function with [simd] intrinsics for parallelism.
*/
void simple_conv2d_simd(const double* input, const double* kernel, double * output, size_t input_size, size_t kernel_size) {
}