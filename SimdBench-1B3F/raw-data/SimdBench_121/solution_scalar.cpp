int add_elements(std::vector<int> arr, int k){
    int sum=0;
    for (int i=0;i<k;i++)
        if( arr[i]>=-99 and arr[i]<=99)
            sum+=arr[i];
    return sum;
}
