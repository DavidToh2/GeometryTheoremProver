#include "StrUtils.hh"

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0, opos = 0, l = delimiter.size();
    std::string token;
    while ((pos = s.find(delimiter, opos)) != std::string::npos) {
        tokens.emplace_back(s.substr(opos, pos - opos));
        opos = pos+l;
    }
    tokens.emplace_back(s.substr(opos, pos - opos));

    return tokens;
}