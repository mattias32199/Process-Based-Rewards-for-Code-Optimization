std::string rounded_avg(int64_t n, int64_t m){
    if (n>m) return "-1";
    
    int64_t sum = 0;
    for(int64_t i = n; i<=m; ++i) {sum += i;}
    sum /= m - n + 1;

    std::string out="";
    while (sum>0){
        out=std::to_string(sum%2)+out;
        sum=sum/2;
    }
    return out;
}