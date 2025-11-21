uint64_t largest_divisor(uint64_t n){
    for (uint64_t i=2;i*i<=n;i++)
        if (n%i==0) return  n/i;
    return 1;
}