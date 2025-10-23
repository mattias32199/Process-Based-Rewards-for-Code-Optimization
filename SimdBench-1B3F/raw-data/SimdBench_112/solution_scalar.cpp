std::vector<int> func(int n) {
    // Create a vector of size n
    std::vector<int> result(n);

    for (int i = 1; i <= n; i++) {
        if (i % 2 == 0) {
            // Calculate factorial for even indices
            int factorial = 1;
            for (int j = 1; j <= i; j++) {
                factorial *= j;
                factorial %= 10000;
            }
            result[i - 1] = factorial;
        } else {
            // Calculate sum of numbers from 1 to i for odd indices
            int sum = i * (i + 1) / 2; // Use the formula for sum of first i natural numbers
            result[i - 1] = sum;
        }
    }

    return result;
}