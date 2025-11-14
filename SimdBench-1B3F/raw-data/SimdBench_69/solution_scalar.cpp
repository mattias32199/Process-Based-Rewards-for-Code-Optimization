std::string longest(const std::vector<std::string>& strings){
    std::string out;
    for (int i=0;i<strings.size();i++)
    {
        if (strings[i].length()>out.length()) out=strings[i];
    }
    return out;
}