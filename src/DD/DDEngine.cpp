
#include <map>
#include <memory>
#include <iostream>
#include <ostream>
#include <string>

#include "DD/DDEngine.hh"
#include "Predicate.hh"
#include "Common/Generator.hh"
#include "Common/Constants.hh"

DDEngine::DDEngine() {
    for (std::string pname : Constants::PREDICATE_NAMES) {
        predicates_by_type[pname] = std::vector<std::string>();
    }
}

void DDEngine::__add_theorem_template_from_text(const std::string s) { 

    std::unique_ptr<Theorem> _thr = std::make_unique<Theorem>(s);
    std::string name = _thr.get()->name;
    theorems.insert({name, std::move(_thr)});
}

void DDEngine::__add_construction_template_from_texts(const std::vector<std::string> v) { 

    std::unique_ptr<Construction> _c = std::make_unique<Construction>(v);
    std::string name = _c.get()->name;
    constructions.insert({name, std::move(_c)});
}

void DDEngine::insert_predicate(std::unique_ptr<Predicate> &&predicate) {
    std::string h_ = predicate.get()->hash;
    predicates.insert({h_, std::move(predicate)});
    recent_predicates.emplace_back(h_);
}

bool DDEngine::has_predicate_by_hash(const std::string hash) {
    return predicates.find(hash) != predicates.end();
}

Generator<Predicate*> DDEngine::get_recent_predicates() {
    for (std::string h : recent_predicates) {
        co_yield predicates.at(h).get();
    }
    recent_predicates.clear();
    co_return;
}

void DDEngine::bfs() {

}



void DDEngine::__print_theorems(std::ostream& os) {
    for (auto& thr : theorems) {
        Theorem* theorem = thr.second.get();
        os << theorem->to_string() << std::endl;
    }
}

void DDEngine::__print_constructions(std::ostream& os) {
    for (auto& c : constructions) {
        Construction* construction = c.second.get();
        os << construction->to_string() << std::endl;
    }
}

void DDEngine::__print_predicates(std::ostream& os) {
    for (auto& p : predicates) {
        Predicate* predicate = p.second.get();
        os << predicate->to_string() << std::endl;
    }
}