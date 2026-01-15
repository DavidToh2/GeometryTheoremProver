
#include "Arg.hh"
#include "Geometry/Object.hh"
#include "Common/StrUtils.hh"


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
    if (!std::holds_alternative<Node*>(arg)) { return nullptr; }
    return static_cast<Point*>(std::get<Node*>(arg));
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
    if (arg.index() == 0) {
        return "EMPTY";
    }
    if (std::holds_alternative<Node*>(arg)) {
        return std::get<Node*>(arg)->name;
    }
    if (std::holds_alternative<Frac>(arg)) {
        return std::get<Frac>(arg).to_string();
    }
    if (std::holds_alternative<char>(arg)) {
        return std::string{std::get<char>(arg)};
    }
    return "0";
}