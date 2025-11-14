std::vector<int> count_up_to(int n) {
    if (n <= 2) {
        return {};
    }
    std::vector<int> out(n);
    int count = 0;
    for (int i = 2; i < n; i++) {
        bool isp = true;
        for (int j = 0; j < count && out[j] * out[j] <= i; j++) {
            if (i % out[j] == 0) {
                isp = false;
                break;
            }
        }

        if (isp) {
            out[count] = i;
            count++;
        }
    }
    out.resize(count);
    return out;
}