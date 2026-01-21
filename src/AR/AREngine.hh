
#pragma once 

#include "DD/Predicate.hh"
#include "Table.hh"
#include "Geometry/Object.hh"
#include "Geometry/Value.hh"
#include "Geometry/Object2.hh"

class DDEngine;

/* AREngine class.

This class features three tables:
- `angle_table`: Manipulates angles by looking at the rotation between their "endlines". Used 
to record and derive `const_angle()`, `eqangle()`, `para()`, and `perp()`
- `ratio_table`: Multiplies or divides length ratios via the logarithm of segment lengths. Used 
to record and derive `const_ratio()`, `eqratio()`, and `cong()`.
- `displacement_table`: Adds or subtracts directed lengths. Used to record `cong()` only. Used
to derive `const_ratio()` and `cong()`.

WARNING: I believe that the ratio table can also be used to derive `const_ratio()`, but this is
not what Google did. KIV whether it works or not.

Note concerning invocation of `add_...()` functions: Every invocation of an `add_...()` 
function is through the corresponding GeometricGraph's DD-predicate-making `make_...()`
function, which only ever works with root nodes. Hence, all `add_...()` implementations
may safely assume that the arguments passed to it will always be root nodes.

Note concerning root `Values`: Any merger of two `Value` nodes must have occured via
the invocation of one of GeometricGraph's `set_...()` functions. These functions are
in turn only invoked by GeometricGraph's DD-predicate-making `make_...()` functions, 
which also invoke the corresponding AREngine function `add_...()`. In other words, all 
mergers are automatically recorded in the AREngine.
This means that it is perfectly fine for the `var_to_...` maps to hold `Values` which are
no longer root, since their associations with their roots are already recorded, and their
roots also necessarily have entries in `var_to_...`.

Note concerning root `Objects` for `Displacements`: Mergers of `Point` and `Line` nodes have
been hotwired to also update the AREngine via the `record_point/line_merger()` functions. These
may incur computational cost. My hope is that the cost is not too high as the displacement 
table SHOULD be relatively empty compared to the other two tables. 
*/
class AREngine {
    struct Displacement {
        Line* l;
        Point* p;
    };
    inline constexpr std::string __get_disp_name(Displacement disp) { 
        return "disp_" + disp.l->name + "_" + disp.p->name; 
    }
public:
    Table angle_table;
    Table ratio_table;
    Table displacement_table;

    std::map<Expr::Var, Direction*> var_to_direction;
    std::map<Expr::Var, Length*> var_to_length;
    std::map<Expr::Var, Displacement> var_to_displacement;

    AREngine() : angle_table("pi"), ratio_table("1"), displacement_table() {};

    inline constexpr Expr::Var __get_var(Direction* d) {
        return var_to_direction.insert({d->name, d}).first->first;
    }
    inline constexpr Expr::Var __get_var(Length* l) {
        return var_to_length.insert({l->name, l}).first->first;
    }
    inline constexpr Expr::Var __get_var(Displacement&& disp) {
        return var_to_displacement.insert({ __get_disp_name(disp), std::move(disp) }).first->first;
    }
    inline constexpr Expr::Var __get_var(Displacement& disp) {
        return var_to_displacement.insert({ __get_disp_name(disp), disp }).first->first;
    }
    inline constexpr Direction* __get_direction(const Expr::Var& var) {
        Direction* d = var_to_direction.at(var);
        var_to_direction.at(var) = NodeUtils::get_root(d);
        return var_to_direction.at(var);
    }
    inline constexpr Length* __get_length(const Expr::Var& var) {
        Length* l = var_to_length.at(var);
        var_to_length.at(var) = NodeUtils::get_root(l);
        return var_to_length.at(var);
    }
    inline constexpr Displacement __get_displacement(const Expr::Var& var) {
        Displacement disp = var_to_displacement.at(var);
        disp.l = NodeUtils::get_root(disp.l);
        disp.p = NodeUtils::get_root(disp.p);
        return disp;
    }

    std::vector<Direction*> __get_directions(const std::vector<Expr::Var>& vars);
    std::vector<Length*> __get_lengths(const std::vector<Expr::Var>& vars);

    void add_constangle(Direction* d1, Direction* d2, float f, Predicate* pred);
    void add_eqangle(Direction* d1, Direction* d2, Direction* d3, Direction* d4, Predicate* pred, int pi_offset = 0);
    void add_para(Direction* d1, Direction* d2, Predicate* pred);
    void add_perp(Direction* d1, Direction* d2, Predicate* pred);

    void add_constratio(Length* l1, Length* l2, float f, Predicate* pred);
    void add_constratio(Length* l1, Length* l2, float m, float n, Predicate* pred);
    void add_eqratio(Length* l1, Length* l2, Length* l3, Length* l4, Predicate* pred);

    void update_point_merger(Point* dest, Point* src, Predicate* pred);
    void update_line_merger(Line* dest, Line* src, Predicate* pred);
    /* Sets segments `s1, s2` to be congruent.
    Warning: This member function does not take the Value node directly, instead taking 
    an Object node. As such, it is imperative that the Segments already have Length nodes 
    created. */
    void add_cong(Segment* s1, Segment* s2, Predicate* pred);
    /* Sets segments `s1, s2` to be congruent. 
    Note: The points `s1, s2` are such that `s1 = p1-m, s2 = m-p2`. In other words, `s1`
    should appear to the left of `s2`. */
    void add_midp(Segment* s1, Segment* s2, Predicate* pred);



    /* Returns all unordered pairs of directions `(d1, d2)` and a double `f` satisfying
    `Angle(d1, d2) = f`. Here, `f` is a decimal between 0 and 180. Perpendicular pairs
    of directions are returned with `f = 90`. */
    Generator<std::tuple<Direction*, Direction*, double, std::vector<Predicate*>>> 
    get_all_constangles_and_why();
    Generator<std::tuple<Direction*, Direction*, Direction*, Direction*, std::vector<Predicate*>>> 
    get_all_eqangles_and_why();
    Generator<std::tuple<Direction*, Direction*, std::vector<Predicate*>>> 
    get_all_paras_and_why();

    Generator<std::tuple<Length*, Length*, double, std::vector<Predicate*>>> 
    get_all_constratios_and_why();
    Generator<std::tuple<Length*, Length*, Length*, Length*, std::vector<Predicate*>>> 
    get_all_eqratios_and_why();

    Generator<std::tuple<Length*, Length*, std::vector<Predicate*>>> 
    get_all_congs_and_why_1();
    Generator<std::tuple<Point*, Point*, Point*, Point*, std::vector<Predicate*>>> 
    get_all_congs_and_why_2();

    /* Derive new predicates. */
    void derive(GeometricGraph& ggraph, DDEngine& dd);

    void reset_problem();
};