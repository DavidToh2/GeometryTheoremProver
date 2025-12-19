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

Only root Value nodes populate their `root_obj2s` sets. */ 
template <std::derived_from<Object2> T>
class Value2 : public Node {
public:
    std::map<T*, Predicate*> obj2s;
    std::set<T*> root_obj2s;

    Value2(std::string name) : Node(name) {}
};


/* Measure class.

A Measure describes how large an `Angle` is. */
class Measure : public Value2<Angle> {
public:
    Measure(std::string name) : Value2(name) {}

    /* Associate the angle `a` with the root measure of `this`, by adding the former to the `obj2s` 
    and `root_obj2s` of the latter. 
    Note: If `a` is already present in `obj2s`, then overwriting by `pred` occurs. */
    void add_angle(Angle* a, Predicate* pred);

    /* Returns all pairs of equal angles associated with this measure. */
    Generator<std::pair<Angle*, Angle*>> all_eq_pairs();
    /* Returns all ordered pairs of equal angles associated with this measure. */
    Generator<std::pair<Angle*, Angle*>> all_eq_pairs_ordered();

    /* Merges the root node of `other` into the root node of `this`. */
    void merge(Measure* other, Predicate* pred);
};

class Fraction : public Value2<Ratio> {

};