#pragma once

#include <map>
#include "Object.hh"

class Object2;

/* Value class.

`objs` need not store root objects. As a result, duplication can occur in the sense that multiple objects may
be present in `objs` with the same root. 

`root_objs` stores the root objects corresponding to the objects in `objs`.

Only root Value nodes populate their `root_objs` sets. */
template <std::derived_from<Object> T>
class Value : public Node {
public:
    std::map<T*, Predicate*> objs;
    std::set<T*> root_objs;
    Value(std::string name) : Node(name) {}
};

/* Direction class.

The most up-to-date records of `perp` and `objs` are always stored by the root node. */
class Direction : public Value<Line> {
public:

    Direction* perp = nullptr;
    Predicate* perp_why = nullptr;
    Direction(std::string name) : Value(name) {}

    /* Note: Assumes that this `Direction*` node is a root node */
    bool __has_perp();
    /* Checks if the root node of `this` has a perp */
    bool has_perp();

    /* Sets `this` 's `perp` and `perp_why` attributes to be `d` and `pred` respectively.
    
    Note: Does not set `d` 's `perp` and `perp_why` attributes. Thus, `__set_perp()` needs to be called
    twice, one for `this` and one for `d`.
    Note: Assumes that `this` and `d` are both root nodes.
    Note: Assumes that `this` and `d` do not already have `perp` set. */
    void __set_perp(Direction *d, Predicate* pred);
    /* Sets the root nodes of `this` and `d` to be perpendicular to each other. 
    
    If `this` already has a `perp`, then `this->perp` is merged into `d`.
    If `d` already has a `perp`, then `d->perp` is merged into `this`. */
    void set_perp(Direction *d, Predicate* pred);

    /* Get the root of the perpendicular of `this`. */
    Direction* __get_perp();
    /* Get the root of the perpendicular of the root of `this`. 
    
    Note: Undefined behaviour if `perp` does not exist. Always check using `has_perp()` first. */
    Direction* get_perp();

    /* Add the root node of `l` to the root node of `this`. 
    
    Note: If the root node of `l` is already in the root node of `this`, then overwriting by `pred` occurs. */
    void add_line(Line* l, Predicate* pred);

    Generator<std::pair<Line*, Line*>> all_para_pairs();
    Generator<std::pair<Line*, Line*>> all_perp_pairs();

    /* Merge the root nodes of `this` and `other`. 
    
    Note: Also merges their `perp` s, if and only if both `perp` s have already been set. */
    void merge(Direction* other, Predicate* pred);
    void __merge_perps(Direction* other, Predicate* pred);
};

class Shape : public Value<Triangle> {
public:
    Shape(std::string name) : Value(name) {}
};