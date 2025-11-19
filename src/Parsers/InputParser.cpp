
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Parsers/InputParser.hh"
#include "DD/DDEngine.hh"
#include "Common/Exceptions.hh"

void InputParser::parse_rules_from_file(std::string rule_filepath, DDEngine &dd) {
    
    if (fbuf.is_open()) { fbuf.close(); }
    fbuf.open(rule_filepath);
    if (fbuf.fail()) {
        throw InvalidTextualInputError("Error: Could not open rule file: " + rule_filepath);
    }

    while (std::getline(fbuf, line)) {
        if (line[0] == '#') {
            continue;
        }
        dd.__add_theorem_template_from_text(line);   
    }
}

void InputParser::parse_constructions_from_file(std::string construction_filepath, DDEngine &dd) {

    if (fbuf.is_open()) { fbuf.close(); }
    fbuf.open(construction_filepath);
    if (fbuf.fail()) {
        throw InvalidTextualInputError("Error: Could not open construction file: " + construction_filepath);
    }

    std::string c_decl, c_pres, c_posts;

    while (std::getline(fbuf, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        c_decl = line;

        std::getline(fbuf, line);
        c_pres = line;

        std::getline(fbuf, line);
        c_posts = line;

        dd.__add_construction_template_from_texts(std::vector<std::string>({c_decl, c_pres, c_posts}));
    }
}

std::string InputParser::extract_problem_from_file(std::string input_filepath, std::string problem_name) {

    if (fbuf.is_open()) { fbuf.close(); }
    fbuf.open(input_filepath);
    if (fbuf.fail()) {
        throw InvalidTextualInputError("Error: Could not open input file: " + input_filepath);
    }
    
    std::string line;
    bool problem_found = false;

    while (std::getline(fbuf, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (line.substr(0, 8) == "problem ") {
            std::string pname = line.substr(8);
            if (pname == problem_name) {
                problem_found = true;
                break;
            }
        }
    }

    if (!problem_found) {
        throw InvalidTextualInputError("Problem " + problem_name + " not found in file " + input_filepath);
    }
    std::getline(fbuf, line);
    return line;
}