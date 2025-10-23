int specialFilter(const std::vector<int> & nums){
    int num=0;
    for (int i=0;i<nums.size();i++)
    if (nums[i]>10)
    {
        std::string w=std::to_string(nums[i]);
        if (w[0]%2==1 and w[w.length()-1]%2==1) num+=1;
    }
    return num;
}
