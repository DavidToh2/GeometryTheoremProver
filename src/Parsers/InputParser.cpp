
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

#include "InputParser.hh"
#include "DD/DDEngine.hh"

void InputParser::parse_rules_from_file(std::string rule_filepath, DDEngine &dd) {
    
    if (fbuf.is_open()) { fbuf.close(); }
    fbuf.open(rule_filepath);

    while (std::getline(fbuf, line)) {
        if (line[0] == '#') {
            continue;
        }
        dd.__add_theorem_from_text(line);   
    }
}