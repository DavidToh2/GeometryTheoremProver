#include <string>

#include "DD/DDEngine.hh"
#include "Parsers/InputParser.hh"
#include "Geometry/GeometricGraph.hh"

class GTPEngine {

public:
    GeometricGraph ggraph;
    DDEngine dd;
    InputParser inputParser;

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

};