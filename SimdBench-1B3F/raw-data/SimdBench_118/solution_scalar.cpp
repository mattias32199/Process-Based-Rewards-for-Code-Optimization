int64_t minSubArraySum(const std::vector<int64_t>& nums){
    int64_t current, min;
    current=nums[0];
    min=nums[0];
    for (int i=1;i<nums.size();i++)
    {
       if (current<0) current=current+nums[i];
       else current=nums[i];
       if (current<min) min=current;
    }
    return min;
}