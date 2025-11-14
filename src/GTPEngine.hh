#include <Geometry/GeometricGraph.hh>
#include <DD/DDEngine.hh>

class GTPEngine {

public:
    GeometricGraph ggraph;
    DDEngine dd;

    GTPEngine(
        std::string input_filepath,
        std::string problem_name,
        std::string rule_filepath,
        std::string construction_filepath,
        std::string output_filepath
    );

};