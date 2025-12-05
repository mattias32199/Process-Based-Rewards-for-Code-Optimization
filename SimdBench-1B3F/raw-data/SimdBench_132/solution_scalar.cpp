uint64_t get_max_triples(uint64_t n) {
    // Pre-allocate the vector `a` of size n
    std::vector<int> a(n);

    // Pre-allocate the `sum` and `sum2` vectors with size (n + 1) × 3, initialized to 0
    std::vector<std::vector<int> > sum(n + 1, std::vector<int>(3, 0));
    std::vector<std::vector<int> > sum2(n + 1, std::vector<int>(3, 0));

    // Fill the vector `a` with the formula (i^2 - i + 1) % 3
    for (int i = 1; i <= n; i++) {
        a[i - 1] = (i * i - i + 1) % 3;
    }

    // Compute the cumulative counts of remainders for `sum`
    for (int i = 1; i <= n; i++) {
        sum[i] = sum[i - 1];  // Copy the previous cumulative counts
        sum[i][a[i - 1]] += 1;  // Increment the count for the current value of `a[i-1]`
    }

    // Perform two rounds of cumulative count updates using `sum2`
    for (int times = 1; times < 3; times++) {
        for (int i = 1; i <= n; i++) {
            sum2[i] = sum2[i - 1];  // Copy the previous cumulative counts

            if (i >= 1) {
                for (int j = 0; j <= 2; j++) {
                    sum2[i][(a[i - 1] + j) % 3] += sum[i - 1][j];
                }
            }
        }

        // Update `sum` with the new cumulative counts from `sum2`
        sum = sum2;

        // Reset `sum2` to zero for the next round
        for (int i = 0; i <= n; i++) {
            sum2[i] = {0, 0, 0};
        }
    }

    // Return the count of triples where the sum is divisible by 3
    return sum[n][0];
}