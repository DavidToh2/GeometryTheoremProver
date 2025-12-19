#include "StrUtils.hh"

namespace StrUtils {

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {

    std::vector<std::string> tokens{};
    if (s.empty()) { return tokens; }
    
    size_t pos = 0, opos = 0, l = delimiter.size();
    std::string token;
    while ((pos = s.find(delimiter, opos)) != std::string::npos) {
        tokens.emplace_back(s.substr(opos, pos - opos));
        opos = pos+l;
    }
    tokens.emplace_back(s.substr(opos, pos - opos));

    return tokens;
}

std::pair<std::string, std::string> split_first(const std::string& s, const std::string& delimiter) {
    
    size_t pos = s.find(delimiter);
    if (pos == std::string::npos) {
        return {s, ""};
    } else {
        return {s.substr(0, pos), s.substr(pos + delimiter.size())};
    }
}

} // namespace StrUtils