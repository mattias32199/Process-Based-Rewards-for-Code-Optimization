std::vector<float> find_closest_elements(std::vector<float> numbers){
    std::vector<float> out={};
    for (int i=0;i<numbers.size();i++)
    for (int j=i+1;j<numbers.size();j++)
        if (out.size()==0 or std::abs(numbers[i]-numbers[j])<std::abs(out[0]-out[1]))
            out={numbers[i],numbers[j]};
    if (out[0]>out[1])
        out={out[1],out[0]};
    return out;
}
