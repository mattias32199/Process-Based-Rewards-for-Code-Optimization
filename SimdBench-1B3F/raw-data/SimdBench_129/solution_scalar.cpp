uint64_t special_factorial(uint64_t n){
    uint64_t fact=1,bfact=1;
    for (int i=1;i<=n;i++){
        fact=fact*i;
        bfact=bfact*fact % 100000;
    }
    return bfact;
}