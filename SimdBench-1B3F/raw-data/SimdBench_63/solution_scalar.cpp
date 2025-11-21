bool below_zero(std::vector<int> operations){
    int num=0;
    for (int i=0;i<operations.size();i++)
    {
        num+=operations[i];
        if (num<0) return true;
    }
    return false;
}
