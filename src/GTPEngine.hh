#include <string>

#include "DD/DDEngine.hh"
#include "AR/AREngine.hh"
#include "Geometry/GeometricGraph.hh"
#include "Parsers/InputParser.hh"

class GTPEngine {

public:
    GeometricGraph ggraph;
    DDEngine dd;
    AREngine ar;
    InputParser inputParser;
    
    std::string rule_filepath;
    std::string construction_filepath;
    std::string input_filepath;
    std::string problem_name;

    GTPEngine(
        std::string rule_filepath,
        std::string construction_filepath
    );

    void load_problem(
        std::string input_filepath,
        std::string problem_name
    );

    void solve(
        int max_steps
    );

    void output(
        std::string output_filepath
    );

    void clear_problem();

};