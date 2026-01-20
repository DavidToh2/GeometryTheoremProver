
#include <string>
#include <variant>

#include "Predicate.hh"
#include "Geometry/GeometricGraph.hh"
#include "Common/Frac.hh"
#include "Common/Utils.hh"
#include "Common/StrUtils.hh"
#include "Common/Exceptions.hh"
#include "Common/Constants.hh"
#include "Common/Generator.hh"
#include "Geometry/Object.hh"





PredicateTemplate::PredicateTemplate(const std::string s, std::map<std::string, Arg*> &argmap) {

    std::vector<std::string> v = StrUtils::split(s, " ");
    if (!Utils::isin(v[0], Constants::PREDICATE_NAMES)) {
        throw InvalidTextualInputError("PredicateTemplate: Invalid predicate name: " + v[0]);
    }
    name = Utils::to_pred_t(v[0]);

    for (auto iter = v.begin() + 1; iter != v.end(); iter++) {
        args.emplace_back(argmap.at(*iter));
    }
}

PredicateTemplate::PredicateTemplate(Predicate* pred, std::vector<std::unique_ptr<Arg>> &arglist) {
    name = pred->name;
    for (const auto& ptr : arglist) {
        args.emplace_back(ptr.get());
    }
}

char PredicateTemplate::set_arg(int i, Node* node) { 
    if (!args.at(i)->empty()) { throw DDInternalError("PredicateTemplate: Attempt to set an already set argument: " + to_string()); }
    return args.at(i)->set(node); 
}
char PredicateTemplate::set_arg(int i, Frac f) { 
    if (!args.at(i)->empty()) { throw DDInternalError("PredicateTemplate: Attempt to set an already set argument: " + to_string()); }
    return args.at(i)->set(f); 
}
char PredicateTemplate::set_arg(int i, char c) { 
    if (!args.at(i)->empty()) { throw DDInternalError("PredicateTemplate: Attempt to set an already set argument: " + to_string()); }
    return args.at(i)->set(c); 
}
bool PredicateTemplate::arg_empty(int i) const noexcept { return args.at(i)->empty(); }
void PredicateTemplate::clear_arg(int i) noexcept { args.at(i)->clear(); }

char PredicateTemplate::set_args(std::vector<Node*> nodes) {
    int i = 0; 
    for (auto node : nodes) {
        if (!args.at(i)->set(node)) { return 0; }
        i++;
    }
    return 1;
}
char PredicateTemplate::set_args(std::vector<Node*> nodes, Frac f) {
    args.at(0)->set(f);
    int i = 1; 
    for (auto node : nodes) {
        if (!args.at(i)->set(node)) { return 0; }
        i++;
    }
    return 1;
}
bool PredicateTemplate::args_filled() const {
    for (auto& argptr : args) {
        if (argptr->empty()) {
            return false;
        }
    }
    return true;
}
void PredicateTemplate::clear_args() {
    for (int i=0; i<args.size(); i++) { 
        args.at(i)->clear(); 
    }
}

Point* PredicateTemplate::get_arg_point(int i) { return args.at(i)->get_point(); }

std::unique_ptr<Predicate> PredicateTemplate::instantiate() {
    return std::make_unique<Predicate>(*this);
}

std::string PredicateTemplate::to_string() const {
    std::string res = Utils::to_pred_str(name);
    for (Arg* arg : args) {
        res = res + " " + arg->to_string();
    }
    return res;
}

std::string PredicateTemplate::to_hash_with_args() const {
    // For now, just use the same as to_string()
    // This is the same as the hash as implemented in the Predicate constructors
    return to_string(); 
}

bool PredicateTemplate::__validate_neq(GeometricGraph &ggraph) {
    Node* node1 = std::get<Node*>(args[0]->arg);
    Node* node2 = std::get<Node*>(args[1]->arg);
    return (NodeUtils::get_root(node1) != NodeUtils::get_root(node2));
}

bool PredicateTemplate::__validate_ncoll(GeometricGraph &ggraph) {
    Point* p1 = static_cast<Point*>(std::get<Node*>(args[0]->arg));
    Point* p2 = static_cast<Point*>(std::get<Node*>(args[1]->arg));
    Point* p3 = static_cast<Point*>(std::get<Node*>(args[2]->arg));

    Line* l = ggraph.__try_get_line(p1, p2);
    if (!l) {
        return true;    
    }
    // Check if p3 is on line l
    return (l->points.find(NodeUtils::get_root(p3)) == l->points.end());
}

bool PredicateTemplate::validate_degeneracy_args(GeometricGraph &ggraph) {
    for (Arg* arg : args) {
        if (!std::holds_alternative<Node*>(arg->arg)) {
            throw DDInternalError("PredicateTemplate: Degeneracy validation failed as argument was not instantiated as a Node: " + to_string());
        }
    }
    if (name == pred_t::DIFF) {
        return __validate_neq(ggraph);
    } else if (name == pred_t::NCOLL) {
        return __validate_ncoll(ggraph);
    } else {
        throw DDInternalError("PredicateTemplate: Degeneracy validation not implemented for predicate: " + Utils::to_pred_str(name));
    }
}





Predicate::Predicate(const pred_t pred_name, std::vector<Node*> &&nodes)
: args(std::move(nodes)), name(pred_name) {
    hash = Utils::to_pred_str(pred_name);
    for (Node* node : args) {
        hash = hash + " " + node->name;
    }
}
Predicate::Predicate(const pred_t pred_name, std::vector<Node*> &&nodes, Frac f)
: Predicate(pred_name, std::move(nodes)) {
    frac_arg = f;
    hash = hash + " " + f.to_string();
}
Predicate::Predicate(const pred_t pred_name, std::vector<Node*> &&nodes, PredVec &&why)
: args(std::move(nodes)), name(pred_name), why(std::move(why)) {
    hash = Utils::to_pred_str(pred_name);
    for (Node* node : args) {
        hash = hash + " " + node->name;
    }
}
Predicate::Predicate(const pred_t pred_name, std::vector<Node*> &&nodes, Frac f, PredVec &&why)
: Predicate(pred_name, std::move(nodes), std::move(why)) {
    frac_arg = f;
    hash = hash + " " + f.to_string();
}
Predicate::Predicate(const pred_t pred_name, std::vector<Node*> &&nodes, std::vector<Predicate*> &&why)
: args(std::move(nodes)), name(pred_name), why(std::move(why)) {
    hash = Utils::to_pred_str(pred_name);
    for (Node* node : args) {
        hash = hash + " " + node->name;
    }
}
Predicate::Predicate(const pred_t pred_name, std::vector<Node*> &&nodes, Frac f, std::vector<Predicate*> &&why)
: Predicate(pred_name, std::move(nodes), std::move(why)) {
    frac_arg = f;
    hash = hash + " " + f.to_string();
}

std::unique_ptr<Predicate> Predicate::from_global_point_map(
    const std::string pred_string, std::map<std::string, std::unique_ptr<Point>> &global_point_map
) {
    std::vector<std::string> v = StrUtils::split(pred_string, " ");
    std::string pred_name = v[0];

    if (!Utils::isin(pred_name, Constants::PREDICATE_NAMES)) {
        throw DDInternalError("Predicate: Invalid predicate name: " + pred_name);
    }

    std::vector<Node*> nodes;
    for (auto iter = v.begin() + 1; iter != v.end(); iter++) {
        std::string pt_str = *iter;
        if (Utils::isinmap(pt_str, global_point_map)) {
            Node* node = global_point_map[pt_str].get();
            nodes.emplace_back(node);
        } else {
            throw DDInternalError("Predicate: Invalid predicate argument: " + pt_str);
        }
    }

    return std::make_unique<Predicate>(Utils::to_pred_t(pred_name), std::move(nodes));
}

Predicate::Predicate(PredicateTemplate &pt) {
    hash = pt.to_hash_with_args();
    name = pt.name;

    for (int i=0; i<pt.args.size(); i++) {
        std::visit( overloaded {
            /* ampersands [&] make capture-by-reference the default for our lambda. In
            other words, all variables outside the lambda can be accessed by reference. */ 
            [&](Node* node) { args.emplace_back(static_cast<Point*>(node)); },
            [&](Frac f) { frac_arg = f; },
            [&](auto&) { throw DDInternalError("Predicate: Invalid argument in predicate template: " + pt.to_string()); }
        }, pt.args[i]->arg );
    }
}

std::string Predicate::to_string() const { return hash; }





void PredVec::operator+=(Predicate* pred) {
    preds.emplace_back(pred);
}
void PredVec::operator+=(const PredVec& other) {
    preds.insert(preds.end(), other.preds.begin(), other.preds.end());
}

std::string PredVec::to_string() const {
    if (preds.empty()) {
        return "EMPTY";
    }
    std::string res = preds[0]->to_string();
    for (auto iter = preds.begin() + 1; iter != preds.end(); ++iter) {
        res = res + " && " + (*iter)->to_string(); 
    }
    return res;
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

        for (auto vIter = v.begin() + 1; vIter != v.end(); vIter++) { pred_args.emplace_back(argmap.at(*vIter)); }

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