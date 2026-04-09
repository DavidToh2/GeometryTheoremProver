
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <iomanip>

#include "DD/Predicate.hh"
#include "Numerics/NumInstance.hh"

class DDEngine;

class OutputParser {

    std::ofstream os;

    std::string __format_predicate(Predicate* pred);
    std::string format_predicate_with_why(Predicate* pred, Predicate* base_pred);
public:

    void set_output_stream(std::string file_name);


    void format_problem_description(std::string problem_name, std::string problem_string);
    void format_numeric_diagram(NumInstance& num_instance);
    void format_failed_numeric_diagram(NumInstance& num_instance);
    void format_solution_from_predset(std::map<int, std::set<Predicate*>>& predset, DDEngine& dd);


    void close_output_stream();
};