#include <ios>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <random>

#include "GTPEngine.hh"
#include "Common/Constants.hh"
#include "DD/Predicate.hh"
#include "AR/AREngine.hh"
#include "Geometry/GeometricGraph.hh"
#include "IO/InputParser.hh"
#include "Common/StrUtils.hh"

std::random_device rd = std::random_device();
std::mt19937 gen(rd());

GTPEngine::GTPEngine(
    std::string rule_filepath,
    std::string construction_filepath,
    std::string profiler_filepath
) {
    this->construction_filepath = construction_filepath;
    this->rule_filepath = rule_filepath;
    this->profiler_filepath = profiler_filepath;

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

    this->ggraph.tr = &tr;
}

bool GTPEngine::load_problem(
    std::string input_filepath,
    std::string problem_name,
    std::string output_filepath
) {

    std::cout << "Loading problem " << problem_name << std::endl;
    
    this->input_filepath = input_filepath;
    this->problem_name = problem_name;
    this->output_filepath = output_filepath;

    outputParser.set_output_stream(output_filepath);
    if (!profiler_filepath.empty()) {
        outputParser.set_profiler_stream(profiler_filepath);
    }

    try {

        // Read in the problem.
        std::string problem_string = inputParser.extract_problem_from_file(input_filepath, problem_name);

        outputParser.format_problem_description(problem_name, problem_string);
        
        auto [_construction_steps, _goal] = StrUtils::split_first(problem_string, "?");
        std::vector<std::string> _construction_stages = StrUtils::split(_construction_steps, ";");

        for (std::string _construction_stage : _construction_stages) {
            StrUtils::trim(_construction_stage);
            /* This function:
            - populates the DDEngine with the initial predicates;
            - populates the GeometricGraph with the initial points only;
            - populates the NumEngine with Numeric's.*/
            Construction::construct_no_checks(_construction_stage, dd, nm, ggraph);
        }

        StrUtils::trim(_goal);
        dd.set_conclusion(Predicate::from_global_point_map(_goal, ggraph.points));

    } catch (const std::exception& e) {
        std::cerr << "Error loading problem: " << e.what() << std::endl;
        return false;
    }

    profiler = Profiler();

    return true;


}

bool GTPEngine::draw() {
    std::cout << "Drawing numeric diagram for problem " << problem_name << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    // Numerically compute and resolve points in the NumEngine.
    bool success = nm.first_draw();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "Time to draw numeric diagram: " << duration << " us" << std::endl;
    profiler.nm_p.duration = duration;
    for (const auto& v : nm.final_inst.params) {
        profiler.nm_p.num_params += v.size();
    }

    if (success) {
        outputParser.format_numeric_diagram(nm.final_inst);
        std::cout << "Numeric diagram drawn successfully!" << std::endl;
    } else {
        outputParser.format_failed_numeric_diagram(nm.final_inst);
        std::cout << "Failed to draw numeric diagram!" << std::endl;
    }

    profiler.num_success = success;
    return success;
}

bool GTPEngine::solve(
    int max_steps
) {
    std::cout << "Solving problem " << problem_name << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    // Add numeric values from the NumEngine
    ggraph.initialise_point_numerics(nm);

    // Add initial geometric objects (lines, circles, directions etc.) from the initial predicates
    ggraph.synthesise_preds(dd, ar);
    int step = 1;

    for (; step <= max_steps; step++) {

        std::cout << "-------- Iteration " << step << ": --------\n";

        auto start_time_ = std::chrono::high_resolution_clock::now();
        dd.search(ggraph, profiler);
        auto end_time_ = std::chrono::high_resolution_clock::now();
        auto duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_time_ - start_time_).count();
        profiler.dd_p.duration.emplace_back(duration_);

        start_time_ = std::chrono::high_resolution_clock::now();
        int dd_num_preds = ggraph.synthesise_preds(dd, ar);
        end_time_ = std::chrono::high_resolution_clock::now();
        duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_time_ - start_time_).count();
        profiler.ggraph_p.duration_dd.emplace_back(duration_);
        profiler.ggraph_p.num_preds_dd.emplace_back(dd_num_preds);

        start_time_ = std::chrono::high_resolution_clock::now();
        ar.derive(ggraph, dd, profiler);
        end_time_ = std::chrono::high_resolution_clock::now();
        duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_time_ - start_time_).count();
        profiler.ar_p.duration.emplace_back(duration_);

        start_time_ = std::chrono::high_resolution_clock::now();
        int ar_num_preds = ggraph.synthesise_ar_preds(dd);
        end_time_ = std::chrono::high_resolution_clock::now();
        duration_ = std::chrono::duration_cast<std::chrono::microseconds>(end_time_ - start_time_).count();
        profiler.ggraph_p.duration_ar.emplace_back(duration_);
        profiler.ggraph_p.num_preds_ar.emplace_back(ar_num_preds);

        profiler.ggraph_p.total_nodes.emplace_back(ggraph.count_nodes());

        std::cout << "Derived " << dd_num_preds << " new predicates from DD and "
                  << ar_num_preds << " new predicates from AR." << std::endl;

        
        /* Check if the conclusion was reached. */
        if (dd.check_conclusion(ggraph)) {
            std::cout << "SOLVED!! Conclusion reached at iteration " << step << "!" << std::endl;
            solved = true;
            break;
        }

        if (dd_num_preds == 0 && ar_num_preds == 0) {
            std::cout << "UNSOLVED!! No new predicates derived." << std::endl;
            break;
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    profiler.ggraph_p.total_duration = duration;
    profiler.ggraph_p.iterations = step;
    std::cout << "Time to solve problem: " << duration << " us" << std::endl;
    
    profiler.solved = solved;
    return solved;
}

bool GTPEngine::get_problem_solution() {

    std::cout << "Outputting solution for problem " << problem_name << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    auto [minimal_predset, success] = tr.get_minimal_predset(dd);
    if (success) std::cout << "Extraction successful!" << std::endl;
    else std::cout << "Extraction failed!" << std::endl;

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "Time to extract solution: " << duration << " us" << std::endl;
    profiler.tr_p.duration = duration;
    profiler.tr_p.solution_depth = minimal_predset.size();
    for (const auto& [i, ps] : minimal_predset) {
        profiler.tr_p.solution_length += ps.size();
    }

    outputParser.format_solution_from_predset(minimal_predset, dd);

    profiler.extracted_solution = success;
    return success;
}

void GTPEngine::output_profiler_data() {
    if (!profiler_filepath.empty()) {
        outputParser.output_profiler_data(problem_name, profiler);
    }
}

void GTPEngine::clear_problem() {

    dd.reset_problem();
    ggraph.reset_problem();
    ar.reset_problem();
    nm.reset_problem();
    tr.reset_problem();

    std::cout << std::endl;

    outputParser.close_output_stream();
    outputParser.close_profiler_stream();
    profiler = Profiler();

    solved = false;
}