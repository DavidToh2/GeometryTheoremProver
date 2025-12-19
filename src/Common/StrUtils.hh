#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace StrUtils {

std::vector<std::string> split(const std::string& s, const std::string& delimiter);

std::pair<std::string, std::string> split_first(const std::string& s, const std::string& delimiter);

inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}

} // namespace StrUtils
