std::vector<std::string> odd_count(const std::vector<std::string> & lst) {
    // Pre-allocate the output vector with the same size as the input
    std::vector<std::string> out(lst.size());
    
    for (int i = 0; i < lst.size(); i++) {
        int sum = 0;

        // Count the number of odd digits in the current string
        for (int j = 0; j < lst[i].length(); j++) {
            if (lst[i][j] >= '0' && lst[i][j] <= '9' && (lst[i][j] - '0') % 2 == 1) {
                sum += 1;
            }
        }

        // Construct the output string with the number of odd digits
        std::string s = "the number of odd elements in the string i of the input.";
        std::string s2 = "";
        for (int j = 0; j < s.length(); j++) {
            if (s[j] == 'i') {
                s2 += std::to_string(sum);
            } else {
                s2 += s[j];
            }
        }

        // Assign the result to the pre-allocated output vector
        out[i] = s2;
    }

    return out;
}