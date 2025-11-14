std::vector<int> minPath(const std::vector<std::vector<int> > & grid, int k) {
    int n = grid.size();
    int x, y, minValue;

    // Find the position of the cell with value 1
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == 1) {
                x = i;
                y = j;
            }
        }
    }

    // Find the smallest neighbor of the cell containing 1
    minValue = INT_MAX;

    if (x > 0 && grid[x - 1][y] < minValue) {
        minValue = grid[x - 1][y];
    }
    if (x < n - 1 && grid[x + 1][y] < minValue) {
        minValue = grid[x + 1][y];
    }
    if (y > 0 && grid[x][y - 1] < minValue) {
        minValue = grid[x][y - 1];
    }
    if (y < n - 1 && grid[x][y + 1] < minValue) {
        minValue = grid[x][y + 1];
    }

    // Pre-allocate the output vector and directly assign values
    std::vector<int> out(k);
    for (int i = 0; i < k; i++) {
        if (i % 2 == 0) {
            out[i] = 1;
        } else {
            out[i] = minValue;
        }
    }

    return out;
}