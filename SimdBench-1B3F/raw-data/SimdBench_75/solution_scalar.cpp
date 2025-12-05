std::vector<float> get_positive(const std::vector<float> & l) {
    std::vector<float> out(l.size());  // Pre-allocate space equal to the input size
    int index = 0;  // Track the current position in the output vector

    for (int i = 0; i < l.size(); i++) {
        if (l[i] > 0) {
            out[index++] = l[i];  // Add positive numbers to the output vector
        }
    }
    // Resize the vector to contain only the positive numbers
    out.resize(index);
    return out;
}