std::vector<int> intersperse(std::vector<int> numbers, int delimeter) {
    if (numbers.empty()) {
        return {};
    }
    size_t output_size = numbers.size() + (numbers.size() - 1);
    std::vector<int> out(output_size);

    size_t out_index = 0;
    for (size_t i = 0; i < numbers.size(); ++i) {
        out[out_index++] = numbers[i];
        if (i != numbers.size() - 1) { 
            out[out_index++] = delimeter;
        }
    }
    return out;
}