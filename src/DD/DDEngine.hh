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

template<typename T>    // "alias declaration"
using ptrmap = std::map<std::string, std::unique_ptr<T>>;
template<typename T>
using ptrvec = std::vector<std::unique_ptr<T>>;

class Theorem;
class Construction;

class DDEngine {

    // Predicates
public:
    DDEngine();
    ptrmap<Predicate> predicates;
    ptrmap<Predicate2> predicate2s;

    std::vector<std::string> recent_predicates;
    std::map<std::string, std::vector<std::string>> predicates_by_type;

    std::unique_ptr<Predicate> conclusion;

    ptrmap<Theorem> theorems;
    ptrmap<Construction> constructions;

    void __add_theorem_template_from_text(const std::string s);
    void __add_construction_template_from_texts(const std::vector<std::string> v);

    void insert_predicate(std::unique_ptr<Predicate> &&predicate);
    bool has_predicate_by_hash(const std::string hash);

    Generator<Predicate*> get_recent_predicates();


    void bfs();


    void __print_theorems(std::ostream& os = std::cout);
    void __print_constructions(std::ostream& os = std::cout);
    void __print_predicates(std::ostream& os = std::cout);
};