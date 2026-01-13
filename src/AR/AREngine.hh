
#pragma once 

#include "DD/Predicate.hh"
#include "Table.hh"
#include "Geometry/Object.hh"
#include "Geometry/Value.hh"
#include "Geometry/Object2.hh"

class DDEngine;

class AREngine {
public:
    Table angle_table;
    Table ratio_table;

    std::map<Expr::Var, Direction*> var_to_direction;
    std::map<Expr::Var, Length*> var_to_length;

    AREngine() : angle_table("pi"), ratio_table("1") {};

    inline constexpr Expr::Var __get_var(Direction* d) {
        return var_to_direction.insert({d->name, d}).first->first;
    }
    inline constexpr Expr::Var __get_var(Length* l) {
        return var_to_length.insert({l->name, l}).first->first;
    }
    inline constexpr Direction* __get_direction(const Expr::Var& var) {
        return var_to_direction.at(var);
    }
    inline constexpr Length* __get_length(const Expr::Var& var) {
        return var_to_length.at(var);
    }
    std::vector<Direction*> __get_directions(const std::vector<Expr::Var>& vars);
    std::vector<Length*> __get_lengths(const std::vector<Expr::Var>& vars);

    void add_const_angle(Direction* d1, Direction* d2, float f, Predicate* pred);
    void add_eqangle(Direction* d1, Direction* d2, Direction* d3, Direction* d4, Predicate* pred);
    void add_eqangle(Angle* a1, Angle* a2, Predicate* pred);
    void add_para(Direction* d1, Direction* d2, Predicate* pred);
    void add_perp(Direction* d1, Direction* d2, Predicate* pred);

    void add_const_ratio(Length* l1, Length* l2, float m, float n, Predicate* pred);
    void add_eqratio(Length* l1, Length* l2, Length* l3, Length* l4, Predicate* pred);
    void add_cong(Length* l1, Length* l2, Predicate* pred);

    /* Returns all unordered pairs of directions `(d1, d2)` and a double `f` satisfying
    `Angle(d1, d2) = f`. Here, `f` is a decimal between 0 and 180. Perpendicular pairs
    of directions are returned with `f = 90`. */
    Generator<std::tuple<Direction*, Direction*, double, std::vector<Predicate*>>> 
    get_all_const_angles_and_why();
    Generator<std::tuple<Direction*, Direction*, Direction*, Direction*, std::vector<Predicate*>>> 
    get_all_eqangles_and_why();
    Generator<std::tuple<Direction*, Direction*, std::vector<Predicate*>>> 
    get_all_paras_and_why();

    Generator<std::tuple<Length*, Length*, double, std::vector<Predicate*>>> 
    get_all_const_ratios_and_why();
    Generator<std::tuple<Length*, Length*, Length*, Length*, std::vector<Predicate*>>> 
    get_all_eqratios_and_why();
    Generator<std::tuple<Length*, Length*, std::vector<Predicate*>>> 
    get_all_congs_and_why();

    /* Derive new predicates. */
    void derive(GeometricGraph& ggraph, DDEngine& dd);

    void reset_problem();
};