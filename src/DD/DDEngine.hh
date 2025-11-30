#pragma once

#include <map>
#include <vector>
#include <memory>
#include <ostream>
#include <iostream>

#include "Predicate.hh"
#include "Predicate2.hh"
#include "Theorem.hh"
#include "Construction.hh"
#include "Common/Generator.hh"
#include "Geometry/GeometricGraph.hh"

template<typename T>    // "alias declaration"
using uptrmap = std::map<std::string, std::unique_ptr<T>>;
template<typename T>
using ptrvec = std::vector<std::unique_ptr<T>>;

class Theorem;
class Construction;

class DDEngine {

    // Predicates
public:
    DDEngine();
    std::unique_ptr<Predicate> base_pred;
    uptrmap<Predicate> predicates;
    uptrmap<Predicate2> predicate2s;

    std::vector<std::string> recent_predicates;
    std::map<std::string, std::vector<std::string>> predicates_by_type;

    std::unique_ptr<Predicate> conclusion;

    uptrmap<Theorem> theorems;
    uptrmap<Construction> constructions;

    void __add_theorem_template_from_text(const std::string s);
    void __add_construction_template_from_texts(const std::vector<std::string> v);

    void insert_predicate(std::unique_ptr<Predicate> &&predicate);
    bool has_predicate_by_hash(const std::string hash);

    Generator<Predicate*> get_recent_predicates();

    /* Predicate matching functions */
    Generator<bool> match_coll(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> match_cyclic(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> match_para(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> match_perp(PredicateTemplate* pred_template, GeometricGraph &ggraph);

    /* Search functions */
    void search(GeometricGraph &ggraph);
    void search2(GeometricGraph &ggraph);

    bool check_conclusion(GeometricGraph &ggraph);

    void __print_theorems(std::ostream& os = std::cout);
    void __print_constructions(std::ostream& os = std::cout);
    void __print_predicates(std::ostream& os = std::cout);
};