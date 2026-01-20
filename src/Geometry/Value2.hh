#pragma once

#include <map>
#include "Node.hh"
#include "Value.hh"
#include "Object2.hh"

class Measure;
class Fraction;


/* Value2 class.

`obj2s` need not store root objects. As a result, duplication can occur in the sense that multiple objects may
be present in `obj2s` with the same root. 

`root_obj2s` stores the root objects corresponding to the objects in `obj2s`.

Only root Value nodes populate their `root_obj2s` sets.

Every Value2 node contains a `Frac val` populating its actual value.. Root nodes are the most up-to-date 
regarding storage of vals - hence, a child node may be associated with a root node with a val, but the
child node may not itself have a val. Node merging does NOT reconcile vals: this should be done manually
by invoking functions. */ 
template <std::derived_from<Object2> T>
class Value2 : public Node {
public:
    std::map<T*, Predicate*> obj2s;
    std::set<T*> root_obj2s;
    Frac val = -1;
    Predicate* val_why = nullptr;

    Value2(std::string name) : Node(name) {}

    constexpr bool has_val() { return (val > -0.9); }
    constexpr void remove_val() { val = -1; val_why = nullptr; }
};


/* Measure class.

A Measure describes how large an `Angle` is.

The actual value (as calculated by the AREngine) of the measure may be stored in the `val` attribute, as an
angle between 0 and 180. */
class Measure : public Value2<Angle> {
public:
    Measure(std::string name) : Value2(name) {}

    /* Associate the angle `a` with the root measure of `this`, by adding the former to the `obj2s` 
    and `root_obj2s` of the latter, as well as updating the `measure` and `measure_why` of the former.
    This is done by calling `Angle::set_measure()`. 
    Note: If `a` is already present in `obj2s`, then overwriting by `pred` occurs. */
    void add_angle(Angle* a, Predicate* pred);

    /* Returns all pairs of equal angles associated with this measure. */
    Generator<std::pair<Angle*, Angle*>> all_eq_pairs();
    /* Returns all ordered pairs of equal angles associated with this measure. */
    Generator<std::pair<Angle*, Angle*>> all_eq_pairs_ordered();

    /* Merges the root node of `other` into the root node of `this`.
    Warning: The `val`s of the two measures are not reconciled. */
    void merge(Measure* other, Predicate* pred);
};

class Fraction : public Value2<Ratio> {
public:
    Fraction(std::string name) : Value2(name) {}

    /* Associate the ratio `r` with the root fraction of `this`, by adding the former to the `obj2s` 
    and `root_obj2s` of the latter, as well as updating the `fraction` and `fraction_why` of the former.
    This is done by calling `Ratio::set_fraction()`.
    Note: If `r` is already present in `obj2s`, then overwriting by `pred` occurs. */
    void add_ratio(Ratio* r, Predicate* pred);

    /* Returns all pairs of equal ratios associated with this fraction. */
    Generator<std::pair<Ratio*, Ratio*>> all_eq_pairs();
    /* Returns all ordered pairs of equal ratios associated with this fraction. */
    Generator<std::pair<Ratio*, Ratio*>> all_eq_pairs_ordered();

    /* Merges the root node of `other` into the root node of `this`. */
    void merge(Fraction* other, Predicate* pred);
};