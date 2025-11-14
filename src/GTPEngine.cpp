
#include <string>

#include "GTPEngine.hh"
#include <Geometry/GeometricGraph.hh>
#include <Parsers/InputParser.hh>

GTPEngine::GTPEngine(
    std::string input_filepath,
    std::string problem_name,
    std::string rule_filepath,
    std::string construction_filepath,
    std::string output_filepath
) {
    dd = DDEngine();
    ggraph = GeometricGraph();

    InputParser inputParser;

    // Read in the constructions and pass them to the DD engine.

    // Read in the problem and pass it to the DD engine.

    // Read in the rules and pass them to the DD engine.
    inputParser.parse_rules_from_file(rule_filepath, dd);

    dd.__print_theorems();
}