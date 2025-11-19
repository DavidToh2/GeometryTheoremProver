#pragma once

#include <string>

namespace Constants {

const std::string DEFAULT_RULE_FILEPATH = "rules.txt";
const std::string DEFAULT_CONSTRUCTION_FILEPATH = "constructions.txt";
const std::string DEFAULT_OUTPUT_FILEPATH = "output.txt";

// Predicate names. Documented in outline.md
const std::string PREDICATE_NAMES[] = {
    "coll", "cyclic", 
    "para", "perp", 
    "cong", 
    "eqangle", "eqratio", 
    "contri", "simtri", "midp",
    "constangle", "constratio",
    "neq", "ncoll", "sameside",
    "convex"
};

} // namespace Constants