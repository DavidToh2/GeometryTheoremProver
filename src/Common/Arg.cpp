
#include "Arg.hh"
#include "Geometry/Object.hh"
#include "Common/StrUtils.hh"
#include "Common/Utils.hh"

void Arg::clear() { arg = std::monostate{}; }
bool Arg::empty() { return (arg.index() == 0); }
bool Arg::filled() { return (arg.index() != 0); }

char Arg::set(Node* node) { 
    if (!empty()) {
        if (std::get<Node*>(arg) != node) {
            return UNSUCCESSFUL_SET;
        }
        return UNCHANGED_SET;
    }
    arg = node;
    return SUCCESSFUL_SET;
}
char Arg::set(Frac f) { 
    if (!empty()) { return UNSUCCESSFUL_SET; }
    arg = f; 
    return SUCCESSFUL_SET;
}
char Arg::set(char c) { 
    if (!empty()) { return UNSUCCESSFUL_SET; }
    arg = c; 
    return SUCCESSFUL_SET;
}

Point* Arg::get_point() {
    /* std::get_if() returns nullptr should Node* not be present */
    if (auto* p = std::get_if<Node*>(&arg)) {
        return static_cast<Point*>(*p);
    }
    return nullptr;
}

void Arg::operator=(Node* node) { arg = node; }
void Arg::operator=(Frac f) { arg = f; }
void Arg::operator=(char c) { arg = c; }

void Arg::populate_args_and_argmap(const std::string s, std::vector<std::unique_ptr<Arg>> &args, std::map<std::string, Arg*> &argmap) {
    
    std::vector<std::string> _args = StrUtils::split(s, " ");
    for (std::string a : _args) {
        if (!argmap.contains(a)) {
            args.emplace_back(std::make_unique<Arg>());
            argmap.insert({a, (args.back()).get()});
        }
    }
}

std::string Arg::to_string() {
    /* ampersands [&] make capture-by-reference the default for our lambda. In
    other words, all variables outside the lambda can be accessed by reference. */ 
    return std::visit(overloaded {
        [](std::monostate) -> std::string { return "EMPTY"; },
        [](Node* node) -> std::string { return node->name; },
        [](Frac f) -> std::string { return f.to_string(); },
        [](char c) -> std::string { return std::string{c}; }
    }, arg);
}