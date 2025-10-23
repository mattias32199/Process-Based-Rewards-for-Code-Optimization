std::vector<int> factorize(int n) {
    // Estimate the maximum size of the factor list
    int max_factors = std::log2(n) + 1;  // A rough upper bound for prime factors
    std::vector<int> out(max_factors);  // Pre-allocate space
    int index = 0;  // Current position in the vector

    for (int i = 2; i * i <= n; i++) {
        while (n % i == 0) {  // While `i` is a factor of `n`
            n /= i;
            out[index++] = i;  // Add factor to the vector
        }
    }
    if (n > 1) {  // If there is a remaining prime factor > sqrt(n)
        out[index++] = n;
    }

    // Resize the vector to match the number of factors found
    out.resize(index);
    return out;
}
