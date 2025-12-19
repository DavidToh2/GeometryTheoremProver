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
    /* Parses rules from a file and adds them to the DDEngine instance. 
    
    Every rule occupies a single line in the file, and is of the format
    
    `[POINTS] : pred1 a1 a2 ..., pred2 b1 b2 ..., etc. => conc`*/
    std::vector<std::string> parse_rules_from_file(std::string rule_filepath);

    /* Parses constructions from a file and adds them to the DDEngine instance. 
    
    Every construction occupies three lines in the file, in the format

    Construction: `construction_name new_arg1 new_arg2 ... : arg1 arg2 ...`

    Preconditions: `pred1, pred2, ...` (could be empty)

    Postconditions: `pred1, pred2, ...` (could be empty)

    Constructions are separated by a blank line.
    */
    std::vector<std::tuple<std::string, std::string, std::string>> parse_constructions_from_file(std::string construction_filepath);

    /* Extracts a given problem from a file and returns it as a string.
    
    See `Outline.md` for more information about how problems are formatted. */
    std::string extract_problem_from_file(std::string input_filepath, std::string problem_name);

    /* Extracts all problem names from a file and returns it as a vector of strings. */
    std::vector<std::string> extract_all_problem_names_from_file(std::string input_filepath);
};