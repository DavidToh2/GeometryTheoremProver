
#include <string>
#include <variant>
#include <iostream>

#include "Predicate.hh"
#include "Numerics/Numerics.hh"
#include "Common/StrUtils.hh"
#include "Common/Exceptions.hh"
#include "Common/Constants.hh"
#include "Common/Utils.hh"
#include "Common/Generator.hh"

void Arg::clear() { arg = std::monostate{}; }
bool Arg::empty() { return (arg.index() == 0); }

void Arg::set(Object* obj) { arg = obj; }
void Arg::set(Frac f) { arg = f; }
void Arg::set(char c) { arg = c; }

void Arg::operator=(Object* obj) { arg = obj; }
void Arg::operator=(Frac f) { arg = f; }
void Arg::operator=(char c) { arg = c; }

void Arg::populate_args_and_argmap(const std::string s, std::vector<std::unique_ptr<Arg>> &args, std::map<std::string, Arg*> &argmap) {
    
    std::vector<std::string> _args = StrUtils::split(s, " ");
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
        return std::get<Object*>(arg)->name;
    }
    if (std::holds_alternative<Frac>(arg)) {
        return std::get<Frac>(arg).to_string();
    }
    if (std::holds_alternative<char>(arg)) {
        return std::string{std::get<char>(arg)};
    }
    return "0";
}

PredicateTemplate::PredicateTemplate(const std::string s, std::map<std::string, Arg*> &argmap) {

    std::vector<std::string> v = StrUtils::split(s, " ");
    name = v[0];
    if (!Utils::isin(name, Constants::PREDICATE_NAMES)) {
        throw InvalidTextualInputError("PredicateTemplate: Invalid predicate name: " + name);
    }

    for (auto iter = v.begin() + 1; iter != v.end(); iter++) {
        args.emplace_back(argmap[*iter]);
    }
}

void PredicateTemplate::set_arg(int i, Object* obj) noexcept { args.at(i)->set(obj); }
void PredicateTemplate::set_arg(int i, Frac f) noexcept { args.at(i)->set(f); }
void PredicateTemplate::set_arg(int i, char c) noexcept { args.at(i)->set(c); }
void PredicateTemplate::clear_arg(int i) noexcept { args.at(i)->clear(); }

void PredicateTemplate::set_args(std::vector<Object*> objs) {
    int i = 0; 
    for (auto obj : objs) {
        args.at(i)->set(obj);
        i++;
    }
}
void PredicateTemplate::set_args(std::vector<Object*> objs, Frac f) {
    args.at(0)->set(f);
    int i = 1; 
    for (auto obj : objs) {
        args.at(i)->set(obj);
        i++;
    }
}
void PredicateTemplate::clear_args() {
    for (int i=0; i<args.size(); i++) { args.at(i)->clear(); }
}

std::unique_ptr<Predicate> PredicateTemplate::instantiate() {
    return std::make_unique<Predicate>(*this);
}

std::string PredicateTemplate::to_string() {
    std::string res = name;
    for (Arg* arg : args) {
        res = res + " " + arg->to_string();
    }
    return res;
}

std::string PredicateTemplate::to_hash_with_args() {
    // For now, just use the same as to_string()
    // This is the same as the hash as implemented in the Predicate constructors
    return to_string(); 
}

Predicate::Predicate(const std::string pred_name, std::vector<Object*> &&objs) {
    args = std::move(objs);
    hash = pred_name;
    for (Object* obj : args) {
        hash = hash + " " + obj->name;
    }
}

std::unique_ptr<Predicate> Predicate::from_global_point_map(const std::string pred_string, std::map<std::string, std::unique_ptr<Point>> &global_point_map) {
    std::vector<std::string> v = StrUtils::split(pred_string, " ");
    std::string pred_name = v[0];

    if (!Utils::isin(pred_name, Constants::PREDICATE_NAMES)) {
        throw DDInternalError("Predicate: Invalid predicate name: " + pred_name);
    }

    std::vector<Object*> objs;
    for (auto iter = v.begin() + 1; iter != v.end(); iter++) {
        std::string pt_str = *iter;
        if (Utils::isinmap(pt_str, global_point_map)) {
            Object* obj = global_point_map[pt_str].get();
            objs.emplace_back(obj);
        } else {
            throw DDInternalError("Predicate: Invalid predicate argument: " + pt_str);
        }
    }

    // Create Predicate from PredicateTemplate
    return std::make_unique<Predicate>(pred_name, std::move(objs));
}

Predicate::Predicate(PredicateTemplate &pt) {
    hash = pt.to_hash_with_args();

    for (Arg* argptr : pt.args) {
        if (std::holds_alternative<Object*>(argptr->arg)) {
            Object* obj = std::get<Object*>(argptr->arg);
            args.emplace_back(obj);

        } else if (std::holds_alternative<Frac>(argptr->arg)) {
            frac_arg = std::get<Frac>(argptr->arg);

        } else {
            throw DDInternalError("Predicate: Invalid argument in predicate template: " + pt.to_string());
        }
    }
}

std::string Predicate::to_string() { return hash; }

Clause::Clause(std::string s, std::map<std::string, Arg*> &argmap) {

    if (s.empty()) { 
        name = "EMPTY";
        return;
    }

    std::vector<std::string> preds = StrUtils::split(s, ", ");

    for (auto iter = preds.begin(); iter != preds.end(); iter++) {
        std::string pr = *iter;
        std::vector<std::string> v = StrUtils::split(pr, " ");
        std::vector<Arg*> pred_args;

        for (auto vIter = v.begin() + 1; vIter != v.end(); vIter++) { pred_args.emplace_back(argmap[*vIter]); }

        if (iter == (preds.end() - 1)) {
            name = name + v[0];
        } else {
            name = name + v[0] + "_";
        }

        predicates.emplace_back(std::make_unique<PredicateTemplate>(v[0], pred_args));
    }
}

bool Clause::is_empty() { return predicates.empty(); }

Generator<std::unique_ptr<Predicate>> Clause::instantiate() {
    for (auto& predptr : predicates) {
        co_yield predptr.get()->instantiate();
    }
}

Generator<std::string> Clause::instantiate_hashes() {
    for (auto& predptr : predicates) {
        co_yield predptr.get()->to_hash_with_args();
    }
}

std::string Clause::to_string() {
    if (this->is_empty()) {
        return "EMPTY";
    }
    std::string res = predicates[0].get()->to_string();
    for (auto iter = predicates.begin() + 1; iter != predicates.end(); ++iter) {
        res = res + ", " + (*iter).get()->to_string(); 
    }
    return res;
}