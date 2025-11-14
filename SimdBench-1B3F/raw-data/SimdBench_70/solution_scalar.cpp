uint64_t greatest_common_divisor(uint64_t a, uint64_t b){
    uint64_t out, m;
    while (true){
        if (a<b) 
        {
            m=a;a=b;b=m;
        }
        a=a%b;
        if (a==0) return b;
    }
}