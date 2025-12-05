std::vector<int> largest_smallest_integers(const std::vector<int>& lst){
    int maxneg=0,minpos=0;
    for (int i=0;i<lst.size();i++)
    {
        if (lst[i]<0 and (maxneg==0 or lst[i]>maxneg)) maxneg=lst[i];
        if (lst[i]>0 and (minpos==0 or lst[i]<minpos)) minpos=lst[i];
    }
    return {maxneg,minpos};
}
