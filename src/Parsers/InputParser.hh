#pragma once 

#include <sstream>
#include <fstream>
#include <string>
#include "DD/DDEngine.hh"

class InputParser {
    
    std::ifstream fbuf;
    std::istringstream sbuf;
    std::string line;

public:
    void parse_rules_from_file(std::string rule_filepath, DDEngine &dd);

};