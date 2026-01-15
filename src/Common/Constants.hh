#pragma once

#include <string>

constexpr double TOL = 1e-9;
constexpr double TOL2 = 1e-6;

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

const std::string NUMERIC_NAMES[] = {
    "free",
    "segment",
    "triangle", "iso_triangle", "r_triangle", "riso_triangle", "equi_triangle", "r_triangle_p", "r_triangle_n",
    "quadrilateral", "rectangle", "square", "square_off", "pentagon", "trapezoid", "eq_trapezoid",
    "line", "line_at_angle", "line_at_angle2",  "line_bisect", "line_para", "line_perp",
    "ray",
    "circle", "circum", "diameter",
    "midpoint",
    "trisegment",
    "mirror", "reflect",
    "angle_eq2", "angle_eq3", "angle_mirror", "angle_bisect", "angle_trisect",
    "tangents",  "common_tangent", "common_tangent2",
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
    LAST    // not used
};

enum class num_t {
    FREE,
    SEGMENT,
    TRIANGLE,
    ISO_TRIANGLE,
    R_TRIANGLE,
    RISO_TRIANGLE,
    EQUI_TRIANGLE,
    R_TRIANGLE_P,
    R_TRIANGLE_N,
    EQUI_TRIANGLE_P,
    QUADRILATERAL,
    RECTANGLE,
    SQUARE,
    SQUARE_OFF,
    PENTAGON,
    TRAPEZOID,
    EQ_TRAPEZOID,
    LINE,
    LINE_AT_ANGLE,
    LINE_AT_ANGLE2,
    LINE_BISECT,
    LINE_PARA,
    LINE_PERP,
    RAY,
    CIRCLE,
    CIRCUM,
    DIAMETER,
    MIDPOINT,
    TRISEGMENT,
    MIRROR,
    REFLECT,
    ANGLE_EQ2,
    ANGLE_EQ3,
    ANGLE_MIRROR,
    ANGLE_BISECT,
    ANGLE_TRISECT,
    TANGENTS,
    COMMON_TANGENT,
    COMMON_TANGENT2,
    LAST    // not used
};