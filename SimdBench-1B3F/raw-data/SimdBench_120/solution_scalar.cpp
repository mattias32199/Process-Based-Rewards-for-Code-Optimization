int64_t solution(const std::vector<int16_t>& lst){
    int64_t sum=0;
    for (int i=0;i*2<lst.size();i++)
        if (lst[i*2]%2==1) sum+=lst[i*2];
    return sum;
}