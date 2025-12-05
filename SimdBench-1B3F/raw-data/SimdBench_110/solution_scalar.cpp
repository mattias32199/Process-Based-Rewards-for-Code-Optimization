std::vector<int> make_a_pile(int n) {
    std::vector<int> out(n);
    out[0] = n;
    for (int i = 1; i < n; i++) {
        out[i] = out[i - 1] + 2;
    }
    return out;
}