int search(std::vector<int> lst) {
    std::unordered_map<int, int> freq;
    for (int num : lst) {
        freq[num]++;
    }

    int max = -1;
    for (const auto& pair : freq) {
        int value = pair.first;
        int count = pair.second;
        if (count >= value && value > max) {
            max = value;
        }
    }
    return max;
}