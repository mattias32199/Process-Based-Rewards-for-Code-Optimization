int prod_signs(const std::vector<int> & arr){
    if (arr.size()==0) return -32768;
    int i,sum=0,prods=1;
    for (i=0;i<arr.size();i++){
        sum+=abs(arr[i]);
        if (arr[i]==0) prods=0;
        if (arr[i]<0) prods=-prods;
    }
   return sum*prods;
}
