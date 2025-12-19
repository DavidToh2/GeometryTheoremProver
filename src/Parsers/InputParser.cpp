
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Parsers/InputParser.hh"
#include "DD/DDEngine.hh"
#include "Common/Exceptions.hh"

std::vector<std::string> InputParser::parse_rules_from_file(std::string rule_filepath) {
    
    if (fbuf.is_open()) { fbuf.close(); }
    fbuf.open(rule_filepath);
    if (fbuf.fail()) {
        throw InvalidTextualInputError("Error: Could not open rule file: " + rule_filepath);
    }

    std::vector<std::string> rules;

    while (std::getline(fbuf, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        rules.push_back(line);
    }
    return rules;
}

std::vector<std::tuple<std::string, std::string, std::string>> 
InputParser::parse_constructions_from_file(std::string construction_filepath) {

    if (fbuf.is_open()) { fbuf.close(); }
    fbuf.open(construction_filepath);
    if (fbuf.fail()) {
        throw InvalidTextualInputError("Error: Could not open construction file: " + construction_filepath);
    }

    std::string c_decl, c_pres, c_posts;
    std::vector<std::tuple<std::string, std::string, std::string>> constructions;

    while (std::getline(fbuf, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        c_decl = line;

        std::getline(fbuf, line);
        c_pres = line;

        std::getline(fbuf, line);
        c_posts = line;

        constructions.emplace_back(std::tuple<std::string, std::string, std::string>{c_decl, c_pres, c_posts});
    }
    return constructions;
}

std::string InputParser::extract_problem_from_file(std::string input_filepath, std::string problem_name) {

    if (fbuf.is_open()) { fbuf.close(); }
    fbuf.open(input_filepath);
    if (fbuf.fail()) {
        throw InvalidTextualInputError("Error: Could not open input file: " + input_filepath);
    }
    
    std::string line;
    std::string problem;
    bool problem_found = false;

    while (std::getline(fbuf, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (line.substr(0, 8) == "problem ") {
            size_t i = line.find('{'), j = line.find(' ', 8);
            std::string pname;
            if (j != std::string::npos) {
                pname = line.substr(8, j - 8);
            } else if (i != std::string::npos) {
                pname = line.substr(8, i - 8);
            } else {
                pname = line.substr(8);
            }
            if (pname == problem_name) {
                problem_found = true;
                problem = line;
                break;
            }
        }
    }

    if (!problem_found) {
        throw InvalidTextualInputError("Problem " + problem_name + " not found in file " + input_filepath);
    }
    while (problem_found) {
        if (!std::getline(fbuf, line)) {
            throw InvalidTextualInputError("Error: Unexpected end of file when reading problem " + problem_name);
        }
        if (line.empty() || line[0] == '#') continue;
        if (line.find('}') != std::string::npos) {
            problem_found = false;
        }
        problem += " " + line;
    }
    size_t i = problem.find('{'), j = problem.find('}');
    return problem.substr(i+1, j-i-1);
}

std::vector<std::string> InputParser::extract_all_problem_names_from_file(std::string input_filepath) {

    if (fbuf.is_open()) { fbuf.close(); }
    fbuf.open(input_filepath);
    if (fbuf.fail()) {
        throw InvalidTextualInputError("Error: Could not open input file: " + input_filepath);
    }
    
    std::string line;
    std::vector<std::string> problem_names;

    while (std::getline(fbuf, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        if (line.substr(0, 8) == "problem ") {
            size_t i = line.find('{'), j = line.find(' ', 8);
            std::string pname;
            if (j != std::string::npos) {
                pname = line.substr(8, j - 8);
            } else if (i != std::string::npos) {
                pname = line.substr(8, i - 8);
            } else {
                pname = line.substr(8);
            }
            problem_names.push_back(pname);
        }
    }

    return problem_names;
}