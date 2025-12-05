int digitSum(std::string s){
    int sum=0;
    for (int i=0;i<s.length();i++)
        if (s[i]>=65 and s[i]<=90)
            sum+=s[i];
    return sum;
}
