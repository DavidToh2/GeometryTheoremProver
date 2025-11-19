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
    void parse_rules_from_file(std::string rule_filepath, DDEngine &dd);

    /* Parses constructions from a file and adds them to the DDEngine instance. 
    
    Every construction occupies three lines in the file, in the format

    Construction: `construction_name new_arg1 new_arg2 ... : arg1 arg2 ...`

    Preconditions: `pred1, pred2, ...` (could be empty)

    Postconditions: `pred1, pred2, ...` (could be empty)

    Constructions are separated by a blank line.
    */
    void parse_constructions_from_file(std::string construction_filepath, DDEngine &dd);

    /* Extracts a given problem from a file and returns it as a string.
    
    A problem occupies two lines in the file. The first line is always the word "problem" followed by the problem name.
    
    The second line is the problem description, in the format
    
    `construction1, construction2, ..., constructionk ? goalpred` */
    std::string extract_problem_from_file(std::string input_filepath, std::string problem_name);
};