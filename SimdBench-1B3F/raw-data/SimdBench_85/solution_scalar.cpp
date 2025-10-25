std::string change_base(int64_t x, int8_t base){
    bool is_negative = false;
    if (x < 0) {
        is_negative = true;
        x = std::abs(x);
    }
    std::string out="";
    if(x==0) out=std::to_string(0);
    while (x>0)
    {
        out=std::to_string(x%base)+out;
        x=x/base;
    }
    if (is_negative) {
        out = "-" + out;
    }
    return out;
}