std::vector<float> derivative(std::vector<float> xs) {
    if (xs.size() <= 1) {
        std::vector<float> out(0);
        return out;
    }
    std::vector<float> out(xs.size() - 1);
    for (std::size_t i = 1; i < xs.size(); i++) {
        out[i - 1] = i * xs[i];
    }
    return out;
}