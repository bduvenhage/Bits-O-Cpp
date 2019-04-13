//! Tokenise a string using the provided delimiters.
std::vector<std::string> tokenise(const std::string& str, const std::string& delimiters = " ", const bool trimEmpty = false) {
    std::vector<std::string> tokens;
    const std::string::size_type length = str.length();
    std::string::size_type lastPos = 0;
    using value_type = typename std::vector<std::string>::value_type;
    using size_type  = typename std::vector<std::string>::size_type;
    
    while(lastPos < (length + 1)) {
        std::string::size_type pos = str.find_first_of(delimiters, lastPos);
        if(pos == std::string::npos) pos = length;
        if (pos != lastPos || !trimEmpty) tokens.push_back(value_type(str.data()+lastPos, size_type(pos-lastPos)));
        lastPos = pos + 1;
    }
    return tokens;
}
