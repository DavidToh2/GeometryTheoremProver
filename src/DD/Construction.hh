
#include <string>
#include <vector>
#include <memory>

#include "Predicate.hh"

class Construction {

public:
    std::string name = "";
    std::vector<std::unique_ptr<Arg>> args_existing;
    std::vector<std::unique_ptr<Arg>> args_new;

    Clause preconditions;
    std::vector<std::unique_ptr<Predicate>> postconditions;

    Construction(const std::string s);
};