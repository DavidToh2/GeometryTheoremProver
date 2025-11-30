
#include <string>
#include <variant>
#include <iostream>

#include "Predicate.hh"
#include "Geometry/GeometricGraph.hh"
#include "Numerics/Numerics.hh"
#include "Common/StrUtils.hh"
#include "Common/Exceptions.hh"
#include "Common/Constants.hh"
#include "Common/Utils.hh"
#include "Common/Generator.hh"

void Arg::clear() { arg = std::monostate{}; }
bool Arg::empty() { return (arg.index() == 0); }
bool Arg::filled() { return (arg.index() != 0); }

void Arg::set(Object* obj) { arg = obj; }
void Arg::set(Frac f) { arg = f; }
void Arg::set(char c) { arg = c; }

Point* Arg::get_point() {
    if (!std::holds_alternative<Object*>(arg)) { return nullptr; }
    return static_cast<Point*>(std::get<Object*>(arg));
}

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
    if (!Utils::isin(v[0], Constants::PREDICATE_NAMES)) {
        throw InvalidTextualInputError("PredicateTemplate: Invalid predicate name: " + v[0]);
    }
    name = Utils::to_pred_t(v[0]);

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
    std::string res = Utils::to_pred_str(name);
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

bool PredicateTemplate::__validate_neq(GeometricGraph &ggraph) {
    Object* obj1 = std::get<Object*>(args[0]->arg);
    Object* obj2 = std::get<Object*>(args[1]->arg);
    return (NodeUtils::get_root(obj1) != NodeUtils::get_root(obj2));
}

bool PredicateTemplate::__validate_ncoll(GeometricGraph &ggraph) {
    Point* p1 = static_cast<Point*>(std::get<Object*>(args[0]->arg));
    Point* p2 = static_cast<Point*>(std::get<Object*>(args[1]->arg));
    Point* p3 = static_cast<Point*>(std::get<Object*>(args[2]->arg));

    Line* l = ggraph.__try_get_line(p1, p2);
    if (!l) {
        return true;    
    }
    // Check if p3 is on line l
    return (l->points.find(NodeUtils::get_root(p3)) == l->points.end());
}

bool PredicateTemplate::validate_degeneracy_args(GeometricGraph &ggraph) {
    for (Arg* arg : args) {
        if (!std::holds_alternative<Object*>(arg->arg)) {
            throw DDInternalError("PredicateTemplate: Degeneracy validation failed as argument was not instantiated as an Object: " + to_string());
        }
    }
    if (name == pred_t::NEQ) {
        return __validate_neq(ggraph);
    } else if (name == pred_t::NCOLL) {
        return __validate_ncoll(ggraph);
    } else {
        throw DDInternalError("PredicateTemplate: Degeneracy validation not implemented for predicate: " + Utils::to_pred_str(name));
    }
}





Predicate::Predicate(const pred_t pred_name, std::vector<Object*> &&objs) {
    args = std::move(objs);
    name = pred_name;
    hash = Utils::to_pred_str(pred_name);
    for (Object* obj : args) {
        hash = hash + " " + obj->name;
    }
}

Predicate::Predicate(const std::string pred_name, std::vector<Object*> &&objs) : Predicate(Utils::to_pred_t(pred_name), std::move(objs)) {}

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
    name = pt.name;

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

void PredVec::emplace_back(Predicate* pred) {
    preds.emplace_back(pred);
}
void PredVec::operator+=(Predicate* pred) {
    preds.emplace_back(pred);
}
void PredVec::operator+=(const PredVec& other) {
    preds.insert(preds.end(), other.preds.begin(), other.preds.end());
}





ClauseTemplate::ClauseTemplate(std::string s, std::map<std::string, Arg*> &argmap) {

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

bool ClauseTemplate::is_empty() { return predicates.empty(); }

Generator<std::unique_ptr<Predicate>> ClauseTemplate::instantiate() {
    for (auto& predptr : predicates) {
        co_yield predptr.get()->instantiate();
    }
}

Generator<std::string> ClauseTemplate::instantiate_hashes() {
    for (auto& predptr : predicates) {
        co_yield predptr.get()->to_hash_with_args();
    }
}

std::string ClauseTemplate::to_string() {
    if (this->is_empty()) {
        return "EMPTY";
    }
    std::string res = predicates[0].get()->to_string();
    for (auto iter = predicates.begin() + 1; iter != predicates.end(); ++iter) {
        res = res + ", " + (*iter).get()->to_string(); 
    }
    return res;
}