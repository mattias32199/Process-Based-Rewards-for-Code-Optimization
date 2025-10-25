bool is_simple_power(uint64_t x, uint64_t n){
    uint64_t p=1, count=0;
    while (p<=x && count<128)
    {
        if (p==x) return true;
        p=p*n; count+=1;
    }
    return false;
}