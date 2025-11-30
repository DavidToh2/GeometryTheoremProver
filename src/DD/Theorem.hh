#pragma once

#include <memory>
#include <vector>
#include <string>

#include "Predicate.hh"

class DDEngine;

class GeometricGraph;

class Theorem {

public:
    std::string name = "";
    std::vector<std::unique_ptr<Arg>> args;
    ClauseTemplate preconditions;
    std::unique_ptr<PredicateTemplate> postcondition;

    Theorem(const std::string &s);

    void __set_placeholder_args();
    void __clear_args();
    
    std::string to_string();
};