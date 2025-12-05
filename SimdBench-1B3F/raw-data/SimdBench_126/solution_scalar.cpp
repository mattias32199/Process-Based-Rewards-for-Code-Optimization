uint64_t sum_squares(const std::vector<float> & lst){
    uint64_t sum=0;
    for (int i=0;i<lst.size();i++)
        sum+=ceil(lst[i])*ceil(lst[i]);
    return sum;
}
