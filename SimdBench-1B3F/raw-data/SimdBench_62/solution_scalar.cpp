bool has_close_elements(const std::vector<double> & numbers, double threshold){
    int i,j;
    
    for (i=0;i<numbers.size();i++)
    for (j=i+1;j<numbers.size();j++)
    if (std::abs(numbers[i]-numbers[j])<threshold)
    return true;

    return false;
}