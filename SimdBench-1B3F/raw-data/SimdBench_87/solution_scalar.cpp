std::string decode_shift(std::string s){
    std::string out;
    int i;
    for (i=0;i<s.length();i++)
    {
        int w=((int)s[i]+21-(int)'a')%26+(int)'a';   
        out=out+(char)w;
    }
    return out;
}
