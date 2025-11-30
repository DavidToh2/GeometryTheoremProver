#include <string>

#include "GTPEngine.hh"
#include "DD/Predicate.hh"
#include "Parsers/InputParser.hh"
#include <Common/StrUtils.hh>
#include <Geometry/GeometricGraph.hh>
#include <vector>

GTPEngine::GTPEngine(
    std::string rule_filepath,
    std::string construction_filepath,
    std::string output_filepath
) {
    dd = DDEngine();
    ggraph = GeometricGraph();

    inputParser = InputParser();

    // Read in the constructions and pass them to the DD engine.
    inputParser.parse_constructions_from_file(construction_filepath, dd);

    // Read in the rules and pass them to the DD engine.
    inputParser.parse_rules_from_file(rule_filepath, dd);
}

void GTPEngine::load_problem(
    std::string input_filepath,
    std::string problem_name
) {
    // Read in the problem and pass it to the DD engine.
    std::string problem_string = inputParser.extract_problem_from_file(input_filepath, problem_name);
    
    std::vector<std::string> _v0 = StrUtils::split(problem_string, " ? ");
    std::vector<std::string> _constructions = StrUtils::split(_v0[0], ", ");
    std::string _goal = _v0[1];

    for (std::string _construction : _constructions) {
        // add construction to ggraph;
        Construction::construct_no_checks(_construction, dd, ggraph);
    }

    dd.conclusion = Predicate::from_global_point_map(_goal, ggraph.points);

    dd.__print_predicates();
    ggraph.__print_points();
}

void GTPEngine::solve(
    int max_steps
) {
    for (int step = 0; step < max_steps; step++) {

        /* Synthesises geometric Objects (Lines, Circles, Angles) based on the recently added 
        predicates "coll", "cyclic". */
        ggraph.synthesise_objects(dd);

        /* Unify all points which have been shown to be identical. */
        // ggraph.unify_points(dd);

        /* Unify all Objects which have been shown to be identical based on the recently added
        predicates "coll", "cyclic". */
        // ggraph.unify_objects(dd);

        /* Link lines which are related based on the recently added predicates "perp", "para". */
        // ggraph.link_lines(dd);


        /* Synthesize Object2 nodes (Angle, Segment, Ratio) based on the recently added predicates
        "eqangle", "eqratio", "constangle", "constratio" and "cong". */
        // ggraph.synthesise_object2s(dd);

        /* Unify all Value nodes (Measure, Length, Fraction) which have been shown to be identical 
        based on the recently added predicates "eqangle", "eqratio". */
        // ggraph.unify_values(dd);


        /* Search on the proof-state graph using the Level 1 Rules to generate new Predicates. */
        dd.search(ggraph);

        /* Search on the proof-state graph using the Level 2 Rules to generate new Predicate2s. */
        // dd.search2(ggraph);

        /* Check if the conclusion was reached. */
        bool res = dd.check_conclusion(ggraph);
    }
}