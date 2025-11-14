int hex_key(const std::string & num){
    std::string key="2357BD";
    int out=0;
    for (int i=0;i<num.length();i++)
    if (find(key.begin(),key.end(),num[i])!=key.end()) out+=1;
    return out;
}
