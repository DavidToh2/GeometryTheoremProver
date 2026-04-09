#include <string>

#include "DD/DDEngine.hh"
#include "AR/AREngine.hh"
#include "Numerics/NumEngine.hh"
#include "Geometry/GeometricGraph.hh"
#include "IO/InputParser.hh"
#include "IO/OutputParser.hh"
#include "Traceback/TracebackEngine.hh"

class GTPEngine {

public:
    DDEngine dd;
    AREngine ar;
    NumEngine nm;
    TracebackEngine tr;
    GeometricGraph ggraph;
    InputParser inputParser;
    OutputParser outputParser;
    
    std::string rule_filepath;
    std::string construction_filepath;
    std::string input_filepath;
    std::string output_filepath;
    std::string problem_name;

    bool solved = false;

    GTPEngine(
        std::string rule_filepath,
        std::string construction_filepath
    );

    bool load_problem(
        std::string input_filepath,
        std::string problem_name,
        std::string output_filepath
    );

    bool draw();

    bool solve(
        int max_steps
    );

    bool get_problem_solution();

    void clear_problem();

};