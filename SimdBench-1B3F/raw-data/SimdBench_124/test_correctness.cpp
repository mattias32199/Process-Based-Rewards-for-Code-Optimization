bool humaneval_check() {
    bool pass = true;
    pass = pass && allclose(minPath_simd({{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}, 3), {1, 2, 1});
    pass = pass && allclose(minPath_simd({{5, 9, 3}, {4, 1, 6}, {7, 8, 2}}, 1), {1});
    pass = pass && allclose(minPath_simd({{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}}, 4), {1, 2, 1, 2});
    pass = pass && allclose(minPath_simd({{6, 4, 13, 10}, {5, 7, 12, 1}, {3, 16, 11, 15}, {8, 14, 9, 2}}, 7), {1, 10, 1, 10, 1, 10, 1});
    pass = pass && allclose(minPath_simd({{8, 14, 9, 2}, {6, 4, 13, 15}, {5, 7, 1, 12}, {3, 10, 11, 16}}, 5), {1, 7, 1, 7, 1});
    pass = pass && allclose(minPath_simd({{11, 8, 7, 2}, {5, 16, 14, 4}, {9, 3, 15, 6}, {12, 13, 10, 1}}, 9), {1, 6, 1, 6, 1, 6, 1, 6, 1});
    pass = pass && allclose(minPath_simd({{12, 13, 10, 1}, {9, 3, 15, 6}, {5, 16, 14, 4}, {11, 8, 7, 2}}, 12), {1, 6, 1, 6, 1, 6, 1, 6, 1, 6, 1, 6});
    pass = pass && allclose(minPath_simd({{2, 7, 4}, {3, 1, 5}, {6, 8, 9}}, 8), {1, 3, 1, 3, 1, 3, 1, 3});
    pass = pass && allclose(minPath_simd({{6, 1, 5}, {3, 8, 9}, {2, 7, 4}}, 8), {1, 5, 1, 5, 1, 5, 1, 5});
    pass = pass && allclose(minPath_simd({{1, 2}, {3, 4}}, 10), {1, 2, 1, 2, 1, 2, 1, 2, 1, 2});
    pass = pass && allclose(minPath_simd({{1, 3}, {3, 2}}, 10), {1, 3, 1, 3, 1, 3, 1, 3, 1, 3});
    return pass;
}

bool correctness_check(int iterations) {
    Random rng;
    for (int i = 0; i < iterations; i++) {
        size_t n = rng.randint<size_t>(2, 16);
        int k = rng.randint<int>(1, 16);
        
        std::vector<int> permutation(n * n);
        for (int i = 0; i < n * n; ++i) {
            permutation[i] = i + 1;
        }
        rng.shuffle(permutation);
        std::vector<std::vector<int>> grid(n, std::vector<int>(n));
        for (size_t idx = 0; idx < permutation.size(); ++idx) {
            size_t row = idx / n;
            size_t col = idx % n;
            grid[row][col] = permutation[idx];
        }
        
        auto result = minPath(grid, k);
        auto result_simd = minPath_simd(grid, k);
        
        if (!allclose(result, result_simd)) {
            if (ERROR_PRINT) std::cerr << "Correctness check failed!" << std::endl;
            return false;
        }
    }
    return true;
}

int main() {
    bool pass = humaneval_check() && correctness_check(ITERATIONS);
    printf("{ \"correctness\": %s }\n", pass ? "1" : "0");
    return 0;
}