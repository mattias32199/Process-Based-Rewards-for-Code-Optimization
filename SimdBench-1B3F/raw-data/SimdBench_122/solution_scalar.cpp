bool is_sorted(const std::vector<int> & lst){
    for (int i=1;i<lst.size();i++)
    {
        if (lst[i]<lst[i-1]) return false;
        if (i>=2 and lst[i]==lst[i-1] and lst[i]==lst[i-2]) return false;
    }
    return true;
}
