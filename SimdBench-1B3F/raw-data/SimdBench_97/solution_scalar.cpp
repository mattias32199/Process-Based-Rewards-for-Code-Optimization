int smallest_change(std::vector<int> arr){
    int out=0;
    for (int i=0;i<arr.size()-1-i;i++)
        if (arr[i]!=arr[arr.size()-1-i])
            out+=1;
    return out;
}
