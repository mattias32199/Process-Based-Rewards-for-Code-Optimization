int max_fill(const std::vector<std::vector<int> > & grid,int capacity){
    int out=0;
    for (int i=0;i<grid.size();i++)
    {
        int sum=0;
        for (int j=0;j<grid[i].size();j++)
            sum+=grid[i][j];
        if (sum>0)  out+=(sum-1)/capacity+1;
    }
    return out;
}