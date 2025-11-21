int count_upper(const std::string & s){
    std::string uvowel="AEIOU";
    int count=0;
    for (int i=0;i*2<s.length();i++)
    if (find(uvowel.begin(),uvowel.end(),s[i*2])!=uvowel.end())
        count+=1;
    return count;
}