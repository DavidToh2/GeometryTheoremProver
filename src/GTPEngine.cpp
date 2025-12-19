#include <string>
#include <vector>
#include <chrono>
#include <fstream>

#include "GTPEngine.hh"
#include "DD/Predicate.hh"
#include "Parsers/InputParser.hh"
#include <Common/StrUtils.hh>
#include <Geometry/GeometricGraph.hh>

GTPEngine::GTPEngine(
    std::string rule_filepath,
    std::string construction_filepath
) {
    dd = DDEngine();
    ggraph = GeometricGraph();

    inputParser = InputParser();

    // Read in the constructions and pass them to the DD engine.
    auto constructions = inputParser.parse_constructions_from_file(construction_filepath);
    for (auto construction : constructions) {
        dd.__add_construction_template_from_texts(construction);
    }

    // Read in the rules and pass them to the DD engine.
    auto rules = inputParser.parse_rules_from_file(rule_filepath);
    for (auto rule : rules) {
        dd.__add_theorem_template_from_text(rule);
    }
}

void GTPEngine::load_problem(
    std::string input_filepath,
    std::string problem_name
) {
    // Read in the problem and pass it to the DD engine.
    std::string problem_string = inputParser.extract_problem_from_file(input_filepath, problem_name);
    
    auto [_construction_steps, _goal] = StrUtils::split_first(problem_string, "?");
    std::vector<std::string> _construction_stages = StrUtils::split(_construction_steps, ";");

    for (std::string _construction_stage : _construction_stages) {
        StrUtils::trim(_construction_stage);
        Construction::construct_no_checks(_construction_stage, dd, ggraph);
    }

    StrUtils::trim(_goal);
    dd.set_conclusion(Predicate::from_global_point_map(_goal, ggraph.points));

    dd.__print_predicates();
    dd.__print_conclusion();
    ggraph.__print_points();
}

void GTPEngine::solve(
    int max_steps
) {
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int step = 0; step < max_steps; step++) {

        std::cout << "Iteration " << step << std::endl;

        /* Synthesises geometric Objects (Lines, Circles, Angles) based on the recently added 
        predicates "coll", "cyclic". */
        ggraph.synthesise_preds(dd);

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

        dd.__print_predicates();

        /* Check if the conclusion was reached. */
        if (dd.check_conclusion(ggraph)) {
            std::cout << "Conclusion reached at iteration " << step << "!" << std::endl;
            break;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "Total time: " << duration << " us" << std::endl;
    return;
}

void GTPEngine::output(std::string output_filepath) {
    std::ofstream fbuf;
    fbuf.open(output_filepath);

    dd.__print_predicates(fbuf);

    fbuf.close();
}

void GTPEngine::clear_problem() {
    // TBA
}