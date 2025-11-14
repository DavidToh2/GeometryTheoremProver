#pragma once

#include <map>
#include <vector>
#include <memory>

#include "Predicate.hh"
#include "Predicate2.hh"
#include "Theorem.hh"
#include "Construction.hh"

template<typename T>    // "alias declaration"
using ptrmap = std::map<std::string, std::unique_ptr<T>>;
template<typename T>
using ptrvec = std::vector<std::unique_ptr<T>>;

class DDEngine {

    // Predicates
public:
    ptrmap<Predicate> predicates;
    ptrmap<Predicate2> predicate2s;

    ptrmap<Theorem> theorems;
    ptrmap<Construction> constructions;

    void __add_theorem_from_text(std::string s);

    void __print_theorems();

};