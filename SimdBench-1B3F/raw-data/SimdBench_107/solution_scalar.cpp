bool check_dict_case(std::map<std::string, std::string> dict) {
    std::map<std::string, std::string>::const_iterator it;
    int islower = 0, isupper = 0;
    if (dict.size() == 0) return false;
    for (it = dict.begin(); it != dict.end(); it++) {
        std::string key = it->first;

        for (size_t i = 0; i < key.size(); i++) {
            if (key[i] < 65 || (key[i] > 90 && key[i] < 97) || key[i] > 122) return false;
            if (key[i] >= 65 && key[i] <= 90) isupper = 1;
            if (key[i] >= 97 && key[i] <= 122) islower = 1;
            if (isupper + islower == 2) return false;
        }
    }
    return true;
}