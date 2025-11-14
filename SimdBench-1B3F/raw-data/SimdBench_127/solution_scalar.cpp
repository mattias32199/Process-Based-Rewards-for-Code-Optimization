int can_arrange(const std::vector<int> & arr){
    int max=-1;
    for (int i=0;i<arr.size();i++)
    if (arr[i]<=i) max=i;
    return max;
}
