
#include <string>

#include "Predicate.hh"
#include <Numerics/Numerics.hh>
#include <Common/StrUtils.hh>

void Arg::clear() { arg = std::monostate{}; }
bool Arg::empty() { return (arg.index() == 0); }

void Arg::set(Object* obj) { arg = obj; }
void Arg::set(Frac f) { arg = f; }
void Arg::set(char c) { arg = c; }

void Arg::populate_args_and_argmap(const std::string s, std::vector<std::unique_ptr<Arg>> &args, std::map<std::string, Arg*> &argmap) {

    std::vector<std::string> _args = split(s, " ");
    for (std::string a : _args) {
        args.emplace_back(std::make_unique<Arg>());
        argmap.insert({a, (args.back()).get()});
    }
}

std::string Arg::to_string() {
    if (arg.index() == 0) {
        return "EMPTY";
    }
    if (std::holds_alternative<Object*>(arg)) {
        return std::get<Object*>(arg)->id;
    }
    if (std::holds_alternative<Frac>(arg)) {
        return std::get<Frac>(arg).to_string();
    }
    if (std::holds_alternative<char>(arg)) {
        return std::string{std::get<char>(arg)};
    }
    return "0";
}

Predicate::Predicate(const std::string s, std::map<std::string, Arg*> &argmap) {

    std::vector<std::string> v = split(s, " ");
    name = v[0];
    for (auto iter = v.begin() + 1; iter != v.end(); iter++) {
        args.emplace_back(argmap[*iter]);
    }
}

std::string Predicate::to_string() {
    std::string res = name;
    for (Arg* arg : args) {
        res = res + " " + arg->to_string();
    }
    return res;
}

void Predicate::set_arg(int i, Object* obj) noexcept { args.at(i)->set(obj); }
void Predicate::set_arg(int i, Frac f) noexcept { args.at(i)->set(f); }
void Predicate::set_arg(int i, char c) noexcept { args.at(i)->set(c); }
void Predicate::clear_arg(int i) noexcept { args.at(i)->clear(); }

void Predicate::set_args(std::vector<Object*> objs) {
    int i = 0; 
    for (auto obj : objs) {
        args.at(i)->set(obj);
        i++;
    }
}
void Predicate::set_args(std::vector<Object*> objs, Frac f) {
    args.at(0)->set(f);
    int i = 1; 
    for (auto obj : objs) {
        args.at(i)->set(obj);
        i++;
    }
}
void Predicate::clear_args() {
    for (int i=0; i<args.size(); i++) { args.at(i)->clear(); }
}

Clause::Clause(std::string s, std::map<std::string, Arg*> &argmap) {

    std::vector<std::string> preds = split(s, ", ");

    for (auto iter = preds.begin(); iter != preds.end(); iter++) {
        std::string pr = *iter;
        std::vector<std::string> v = split(pr, " ");
        std::vector<Arg*> pred_args;

        for (auto vIter = v.begin() + 1; vIter != v.end(); vIter++) { pred_args.emplace_back(argmap[*vIter]); }

        if (iter == (preds.end() - 1)) {
            name = name + v[0];
        } else {
            name = name + v[0] + "_";
        }

        predicates.emplace_back(std::make_unique<Predicate>(v[0], pred_args));
    }
}

std::string Clause::to_string() {
    std::string res = predicates[0].get()->to_string();
    for (auto iter = predicates.begin() + 1; iter != predicates.end(); ++iter) {
        res = res + ", " + (*iter).get()->to_string(); 
    }
    return res;
}