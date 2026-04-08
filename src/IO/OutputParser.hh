
#include <map>
#include <set>
#include <sstream>

#include "DD/Predicate.hh"

class DDEngine;

class OutputParser {

    std::string __format_predicate(Predicate* pred);
    std::string format_predicate_with_why(Predicate* pred, Predicate* base_pred);
public:
    void format_preamble(std::ostream &os);
    void format_problem_description(std::string problem_name, std::string problem_string, std::ostream &os);
    void format_solution_from_predset(std::map<int, std::set<Predicate*>>& predset, DDEngine& dd, std::ostream &os);
};