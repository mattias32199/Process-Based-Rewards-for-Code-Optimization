std::vector<int> pluck(std::vector<int> arr){
    std::vector<int> out(0);
    for (int i=0;i<arr.size();i++)
    if (arr[i]%2==0 and (out.size()==0 or arr[i]<out[0]))
        out={arr[i],i};
    return out;
}
