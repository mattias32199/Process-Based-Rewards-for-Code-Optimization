std::string string_xor(std::string a, std::string b){
    std::string output="";
    for (int i=0;(i<a.length() and i<b.length());i++)
    {
        if (i<a.length() and i<b.length())
        {
            if (a[i]== b[i]) 
            {
                output+='0';
            }  
            else output+='1';
        }
        else
        {
            if (i>=a.length()) 
            {
            output+=b[i];
            }
            else output+=a[i];
        }
    }
    return output;
}
