
#include "Numerics.hh"
#include "Common/StrUtils.hh"
#include "Common/Exceptions.hh"
#include "Common/Utils.hh"

NumericTemplate::NumericTemplate(const std::string outs, const std::string num, std::map<std::string, Arg*> &argmap) {
    std::vector<std::string> v = StrUtils::split(num, " ");
    if (!Utils::isin(v[0], Constants::NUMERIC_NAMES)) {
        throw InvalidTextualInputError("NumericTemplate: Invalid numeric name: " + v[0]);
    }
    name = Utils::to_num_t(v[0]);

    for (auto iter = v.begin() + 1; iter != v.end(); iter++) {
        this->args.emplace_back(argmap.at(*iter));
    }
    v = StrUtils::split(outs, " ");
    for (auto iter = v.begin(); iter != v.end(); iter++) {
        this->outs.emplace_back(argmap.at(*iter));
    }
}

char NumericTemplate::set_arg(int i, Node* node) noexcept { return args.at(i)->set(node); }
bool NumericTemplate::arg_empty(int i) const noexcept { return args.at(i)->empty(); }
void NumericTemplate::clear_arg(int i) noexcept { args.at(i)->clear(); }
Point* NumericTemplate::get_arg_point(int i) const { return args.at(i)->get_point(); }

char NumericTemplate::set_args(std::vector<Node*> nodes) {
    int i = 0; 
    for (auto node : nodes) {
        if (!args.at(i)->set(node)) { return 0; }
        i++;
    }
    return 1;
}
bool NumericTemplate::args_filled() const {
    for (auto& argptr : args) {
        if (argptr->empty()) {
            return false;
        }
    }
    return true;
}
void NumericTemplate::clear_args() {
    for (int i=0; i<args.size(); i++) { 
        args.at(i)->clear(); 
    }
}
bool NumericTemplate::no_args() { return args.empty(); }

char NumericTemplate::set_out(int i, Node* node) noexcept { return outs.at(i)->set(node); }
bool NumericTemplate::out_empty(int i) const noexcept { return outs.at(i)->empty(); }
void NumericTemplate::clear_out(int i) noexcept { outs.at(i)->clear(); }
Point* NumericTemplate::get_out_point(int i) const { return outs.at(i)->get_point(); }

char NumericTemplate::set_outs(std::vector<Node*> nodes) {
    int i = 0; 
    for (auto node : nodes) {
        if (!outs.at(i)->set(node)) { return 0; }
        i++;
    }
    return 1;
}
bool NumericTemplate::outs_filled() const {
    for (auto& outptr : outs) {
        if (outptr->empty()) {
            return false;
        }
    }
    return true;
}
void NumericTemplate::clear_outs() {
    for (int i=0; i<outs.size(); i++) { 
        outs.at(i)->clear(); 
    }
}

std::unique_ptr<Numeric> NumericTemplate::instantiate() {
    return std::make_unique<Numeric>(*this);
}


std::string NumericTemplate::to_string() const {
    std::string res = outs[0]->to_string();
    for (int i=1; i<outs.size(); i++) {
        res = res + " " + outs[i]->to_string();
    }
    res += " = " + Utils::to_num_str(name);
    for (Arg* arg : args) {
        res = res + " " + arg->to_string();
    }
    return res;
}
std::string NumericTemplate::to_hash_with_args() const {
    return to_string();
}



Numeric::Numeric(const std::string s, std::map<std::string, std::unique_ptr<Point>> &global_point_map) {
    hash = s;
    auto [s0, s1]= StrUtils::split_first(s, "=");
    StrUtils::trim(s0);
    StrUtils::trim(s1);
    std::vector<std::string> v0 = StrUtils::split(s0, " ");
    std::vector<std::string> v1 = StrUtils::split(s1, " ");
    if (!Utils::isin(v1[0], Constants::NUMERIC_NAMES)) {
        throw InvalidTextualInputError("NumericTemplate: Invalid numeric name: " + v1[0]);
    }
    name = Utils::to_num_t(v1[0]);

    for (auto iter = v1.begin() + 1; iter != v1.end(); iter++) {
        this->args.emplace_back(global_point_map[*iter].get());
    }
    for (auto iter = v0.begin(); iter != v0.end(); iter++) {
        this->outs.emplace_back(global_point_map[*iter].get());
    }
}
Numeric::Numeric(const NumericTemplate &nt) {
    hash = nt.to_hash_with_args();
    name = nt.name;

    for (int i=0; i<nt.args.size(); i++) {
        args.emplace_back(nt.get_arg_point(i));
    }
    for (int i=0; i<nt.outs.size(); i++) {
        outs.emplace_back(nt.get_out_point(i));
    }
}
bool Numeric::is_base_numeric() {
    return args.empty();
}
std::string Numeric::to_string() const {
    return hash;
}