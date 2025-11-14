uint64_t digits(uint64_t n){
    uint64_t prod=1,has=0;
    std::string s=std::to_string(n);
    for (int i=0;i<s.length();i++)
        if (s[i]%2==1) 
        {
            has=1;
            prod=prod*(s[i]-48);
        }
    if (has==0) return 0;
    return prod;
}
