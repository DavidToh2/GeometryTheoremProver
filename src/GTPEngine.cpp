#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <random>

#include "GTPEngine.hh"
#include "DD/Predicate.hh"
#include "AR/AREngine.hh"
#include "Geometry/GeometricGraph.hh"
#include "Parsers/InputParser.hh"
#include "Common/StrUtils.hh"

std::random_device rd;
std::mt19937 gen(rd());

GTPEngine::GTPEngine(
    std::string rule_filepath,
    std::string construction_filepath
) {
    this->construction_filepath = construction_filepath;
    this->rule_filepath = rule_filepath;

    // Read in the constructions and pass them to the DD engine.
    auto constructions = inputParser.parse_constructions_from_file(construction_filepath);
    for (auto construction : constructions) {
        dd.add_construction_template_from_texts(construction);
    }

    // Read in the rules and pass them to the DD engine.
    auto rules = inputParser.parse_rules_from_file(rule_filepath);
    for (auto rule : rules) {
        dd.add_theorem_template_from_text(rule);
    }
}

void GTPEngine::load_problem(
    std::string input_filepath,
    std::string problem_name
) {
    this->input_filepath = input_filepath;
    this->problem_name = problem_name;

    // Read in the problem and pass it to the DD engine.
    std::string problem_string = inputParser.extract_problem_from_file(input_filepath, problem_name);
    
    auto [_construction_steps, _goal] = StrUtils::split_first(problem_string, "?");
    std::vector<std::string> _construction_stages = StrUtils::split(_construction_steps, ";");

    for (std::string _construction_stage : _construction_stages) {
        StrUtils::trim(_construction_stage);
        Construction::construct_no_checks(_construction_stage, dd, nm, ggraph);
    }

    StrUtils::trim(_goal);
    dd.set_conclusion(Predicate::from_global_point_map(_goal, ggraph.points));

    // dd.__print_predicates();
    // dd.__print_conclusion();
}

void GTPEngine::solve(
    int max_steps
) {
    std::cout << "Solving problem " << problem_name << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    ggraph.synthesise_preds(dd, ar);

    for (int step = 0; step < max_steps; step++) {

        std::cout << "-------- Iteration " << step << ": --------\n";

        dd.search(ggraph);

        int dd_num_preds = ggraph.synthesise_preds(dd, ar);

        ar.derive(ggraph, dd);

        int ar_num_preds = ggraph.synthesise_ar_preds(dd);

        // dd.__print_predicates();
        // ggraph.print();

        std::cout << "Derived " << dd_num_preds << " new predicates from DD and "
                  << ar_num_preds << " new predicates from AR." << std::endl;

        /* Check if the conclusion was reached. */
        if (dd.check_conclusion(ggraph)) {
            std::cout << "SOLVED!! Conclusion reached at iteration " << step << "!" << std::endl;
            break;
        }

        if (dd_num_preds == 0 && ar_num_preds == 0) {
            std::cout << "UNSOLVED!! No new predicates derived." << std::endl;
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

    fbuf << "Problem: " << problem_name << std::endl;
    dd.__print_predicates(fbuf);
    ggraph.print(fbuf);
    fbuf << std::endl;

    fbuf.close();
}

void GTPEngine::clear_problem() {
    dd.reset_problem();
    ggraph.reset_problem();
    ar.reset_problem();
}