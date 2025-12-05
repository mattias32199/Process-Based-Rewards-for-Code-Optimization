std::string solve(uint64_t N){
    std::string str,bi="";
    str=std::to_string(N);
    int i,sum=0;
    for (int i=0;i<str.length();i++)
        sum+=str[i]-48;
    while (sum>0)
    {
        bi=std::to_string(sum%2)+bi;
        sum=sum/2;
    }
    return bi;
}