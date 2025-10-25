std::vector<float> sort_even(std::vector<float> l) {
    int even_size = (l.size() + 1) / 2;
    std::vector<float> even(even_size);

    for (int i = 0; i < even_size; i++) {
        even[i] = l[i * 2];
    }

    std::sort(even.begin(), even.end());
    std::vector<float> out(l.size());
    for (int i = 0; i < l.size(); i++) {
        if (i % 2 == 0) {
            out[i] = even[i / 2];
        } else {
            out[i] = l[i];
        }
    }

    return out;
}