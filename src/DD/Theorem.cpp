
#include "Theorem.hh"
#include <Common/StrUtils.hh>
#include <Geometry/GeometricGraph.hh>

#define DEBUG_THEOREM 1

#if DEBUG_THEOREM
    #define LOG(x) do {std::cout << x << std::endl;} while(0)
#else 
    #define LOG(x)
#endif

Theorem::Theorem(const std::string &s) {

    std::map<std::string, Arg*> argmap;

    std::vector<std::string> m0 = StrUtils::split(s, " => ");
    std::vector<std::string> m1 = StrUtils::split(m0[0], " : ");

    std::string _points = m1[0];
    Arg::populate_args_and_argmap(_points, args, argmap);

    std::string _preconditions = m1[1];
    preconditions = ClauseTemplate(_preconditions, argmap);

    std::string _conclusion = m0[1];
    postcondition = std::make_unique<PredicateTemplate>(_conclusion, argmap);

    name = preconditions.name + "_" + Utils::to_pred_str(postcondition.get()->name);
};

std::unique_ptr<Predicate> Theorem::instantiate_postcondition() {
    LOG("Instantiating: " << to_string());
    std::unique_ptr<Predicate> pred = postcondition.get()->instantiate();
    return pred;
}

void Theorem::__set_placeholder_args() {
    char c = 'a';
    for (auto& argptr : args) {
        argptr.get()->set(c);
        c++;
    }
}

void Theorem::__clear_args() {
    for (auto& argptr : args) { argptr.get()->clear(); }
}

std::string Theorem::to_string() {
    std::string s = preconditions.to_string() + " => " + postcondition.get()->to_string();
    return s;
}
std::string Theorem::to_string_with_placeholders() {
    __set_placeholder_args();
    std::string s = preconditions.to_string() + " => " + postcondition.get()->to_string();
    __clear_args();
    return s;
}