#pragma once

#include <string>

namespace Constants {

const std::string DEFAULT_RULE_FILEPATH = "rules.txt";
const std::string DEFAULT_CONSTRUCTION_FILEPATH = "constructions.txt";
const std::string DEFAULT_OUTPUT_FILEPATH = "output.txt";

// Predicate names. Documented in outline.md
const std::string PREDICATE_NAMES[] = {
    "base",
    "coll", "cyclic", 
    "para", "perp", 
    "cong", 
    "eqangle", "eqratio", 
    "contri", "simtri", "midp", "circle",
    "constangle", "constratio",
    "diff", "ncoll", "npara",
    "sameside", "convex",
    "last"
};

} // namespace Constants

enum class pred_t {
    BASE,
    COLL,
    CYCLIC,
    PARA,
    PERP,
    CONG,
    EQANGLE,
    EQRATIO,
    CONTRI,
    SIMTRI,
    MIDP,
    CIRCLE,
    CONSTANGLE,
    CONSTRATIO,
    DIFF,
    NCOLL,
    NPARA,
    SAMESIDE,
    CONVEX,
    LAST // "sentinel value", not to be used
};