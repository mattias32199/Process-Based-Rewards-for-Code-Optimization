std::vector<int> sort_third(std::vector<int> l) {
    int third_size = (l.size() + 2) / 3;
    std::vector<int> third(third_size);
    for (int i = 0; i < third_size; i++) {
        third[i] = l[i * 3];
    }
    std::sort(third.begin(), third.end());
    std::vector<int> out(l.size());
    for (int i = 0; i < l.size(); i++) {
        if (i % 3 == 0) {
            out[i] = third[i / 3];
        } else {
            out[i] = l[i];
        }
    }
    return out;
}