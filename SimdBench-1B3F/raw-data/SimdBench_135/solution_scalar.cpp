std::vector<int> compare(const std::vector<int>& game, const std::vector<int>& guess) {
    std::vector<int> out(game.size());
    for (int i = 0; i < game.size(); i++) {
        out[i] = std::abs(game[i] - guess[i]);
    }
    return out;
}