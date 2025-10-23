std::vector<int> rolling_max(std::vector<int> numbers) {
    if (numbers.empty()) {
        return {};
    }
    std::vector<int> out(numbers.size());
    int max = numbers[0];
    out[0] = max;
    for (size_t i = 1; i < numbers.size(); ++i) {
        max = std::max(max, numbers[i]);
        out[i] = max;
    }
    return out;
}