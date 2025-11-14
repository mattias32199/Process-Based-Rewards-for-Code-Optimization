uint64_t double_the_difference(const std::vector<float> & lst){
    uint64_t sum=0;
    for (int i=0;i<lst.size();i++)
    if (lst[i]-round(lst[i])<1e-4)
    if (lst[i]>0 and (int)(round(lst[i]))%2==1) sum+=(int)(round(lst[i]))*(int)(round(lst[i]));
    return sum;
}