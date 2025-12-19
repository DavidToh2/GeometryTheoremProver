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
#include "Common/Constants.hh"
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

    std::vector<Predicate*> recent_predicates;
    std::map<pred_t, std::set<Predicate*>> predicates_by_type;

    // Hacky way to implement the conclusion. Doing it as a PredicateTemplate lets us reuse the matching functions.
    std::unique_ptr<PredicateTemplate> conclusion;
    std::vector<std::unique_ptr<Arg>> conclusion_args;

    uptrmap<Theorem> theorems;
    uptrmap<Construction> constructions;

    void __add_theorem_template_from_text(const std::string s);
    void __add_construction_template_from_texts(const std::tuple<std::string, std::string, std::string> v);
    void set_conclusion(std::unique_ptr<Predicate> predicate);

    void insert_predicate(std::unique_ptr<Predicate> &&predicate);
    bool has_predicate_by_hash(const std::string hash);

    Generator<Predicate*> get_recent_predicates();

    /* Predicate matching functions */
    Generator<bool> match_coll(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> match_cyclic(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> match_para(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> match_perp(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> __match_eqangle(PredicateTemplate* pred_template, GeometricGraph &ggraph, int i, std::array<Direction*, 4> &ls);
    Generator<bool> match_eqangle(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> match_circle(PredicateTemplate* pred_template, GeometricGraph &ggraph);

    Generator<bool> match_diff(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> match_ncoll(PredicateTemplate* pred_template, GeometricGraph &ggraph);
    Generator<bool> match_npara(PredicateTemplate* pred_template, GeometricGraph &ggraph);

    // Map from predicate type to matching function
    std::map<pred_t, Generator<bool>(DDEngine::*)(PredicateTemplate*, GeometricGraph &)> match_function_map = {
        {pred_t::COLL, &DDEngine::match_coll},
        {pred_t::CYCLIC, &DDEngine::match_cyclic},
        {pred_t::PARA, &DDEngine::match_para},
        {pred_t::PERP, &DDEngine::match_perp},
        {pred_t::EQANGLE, &DDEngine::match_eqangle},
        {pred_t::CIRCLE, &DDEngine::match_circle},
        {pred_t::DIFF, &DDEngine::match_diff},
        {pred_t::NCOLL, &DDEngine::match_ncoll},
        {pred_t::NPARA, &DDEngine::match_npara}
    };

    Generator<bool> match(Theorem* theorem, int i, int n, GeometricGraph &ggraph);

    /* Search functions */
    void search(GeometricGraph &ggraph);
    void search2(GeometricGraph &ggraph);

    bool check_conclusion(GeometricGraph &ggraph);

    void __print_theorems(std::ostream& os = std::cout);
    void __print_constructions(std::ostream& os = std::cout);
    void __print_predicates(std::ostream& os = std::cout);
    void __print_conclusion(std::ostream& os = std::cout);
};