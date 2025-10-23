void simple_conv2d(const double* input, const double* kernel, double* output, size_t input_size, size_t kernel_size) {
    size_t output_size = input_size - kernel_size + 1;

    for (size_t i = 0; i < output_size; ++i) {
        for (size_t j = 0; j < output_size; ++j) {
            double sum = 0.0;

            for (size_t ki = 0; ki < kernel_size; ++ki) {
                for (size_t kj = 0; kj < kernel_size; ++kj) {
                    size_t input_row = i + ki;
                    size_t input_col = j + kj;
                    size_t input_index = input_row * input_size + input_col;
                    size_t kernel_index = ki * kernel_size + kj;
                    
                    sum += input[input_index] * kernel[kernel_index];
                }
            }

            // ReLU activation
            if (sum < 0) { sum = 0.0; }
            output[i * output_size + j] = sum;
        }
    }
}