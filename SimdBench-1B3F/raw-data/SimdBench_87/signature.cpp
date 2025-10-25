/* 
encode_shift: returns encoded string by shifting every character by 5 in the alphabet.
std::string encode_shift(std::string s){
    std::string out;
    int i;
    for (i=0;i<s.length();i++)
    {
        int w=((int)s[i]+5-(int)'a')%26+(int)'a';   
        out=out+(char)w;
    }
    return out;
}

decode_shift_simd: takes as input string encoded with encode_shift function. Returns decoded string.
The requirement is to implement the function using [simd] intrinsics to achieve parallelism.
*/
std::string decode_shift_simd(std::string s){
}