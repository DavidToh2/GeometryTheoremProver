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
    std::set<T*> root_objs;

    Value(std::string name) : Node(name) {}
};

/* Direction class.

A Direction indicates the orientation of a `Line` (with respect to some fixed frame of reference).

The most up-to-date records of `perp` and `root_objs` are always stored by the root node. 

Every direction stores the set of `angle` s for which it is the `direction1` and `direction2` respectively.
These must always be root `angle` s. Only root directions populate these sets. */
class Direction : public Value<Line> {
public:

    Direction* perp = nullptr;
    std::set<Angle*> on_angles_1;
    std::set<Angle*> on_angles_2;

    Direction(std::string name) : Value(name) {}

    /* Note: Assumes that this `Direction*` node is a root node */
    bool __has_perp();
    /* Checks if the root node of `this` has a perp */
    bool has_perp();

    /* Sets `this->perp = d`.
    Note: Does not set `d->perp`. Thus, `__set_perp()` needs to be called twice, one for `this` 
    and one for `d`.
    Note: Assumes that `this` and `d` are both root nodes.
    Note: Assumes that `this` does not already have `perp` set: otherwise its old `perp` is
    overwritten. */
    void __set_perp(Direction *d);
    /* Sets `this` and `d` to be perpendicular to each other. 
    If `this` already has a `perp`, then `this->perp` is overwritten by `d`.
    If `d` already has a `perp`, then `d->perp` is overwritten by `this`.
    Note: To ensure correct merging behaviour, extract the old `perp`s of `this` and `d`, then merge
    them after the set is done. */
    void set_perp(Direction *d);

    /* Get the root of the perpendicular of `this`. */
    Direction* __get_perp();
    /* Get the root of the perpendicular of the root of `this`. 
    Note: Undefined behaviour if `perp` does not exist. Always check using `has_perp()` first. */
    Direction* get_perp();

    /* Associate the line `l` with the root direction of `this`, by adding the former to the `objs` 
    and `root_objs` of the latter. 
    Calls `Line::set_direction()`, which does the exact same thing.
    Note: Assumes that `l` does not already have a direction. Code using this function should manually 
    check if `l` already has a direction.
    Note: If `l` is already present in `objs`, then nothing happens. */
    void add_line(Line* l);

    /* Returns all pairs of parallel lines associated with this direction. */
    Generator<std::pair<Line*, Line*>> all_para_pairs();
    /* Returns all ordered pairs of parallel lines associated with this direction. */
    Generator<std::pair<Line*, Line*>> all_para_pairs_ordered();

    /* Returns all pairs of perpendicular lines associated with this direction. */
    Generator<std::pair<Line*, Line*>> all_perp_pairs();
    /* Returns all ordered pairs of perpendicular lines associated with this direction. */
    Generator<std::pair<Line*, Line*>> all_perp_pairs_ordered();

    /* Returns all root angles for which the root of this direction is a `direction1` */
    Generator<Angle*> on_angles_as_direction1();
    /* Returns all root angles for which the root of this direction is a `direction2` */
    Generator<Angle*> on_angles_as_direction2();

    /* Merge the root nodes of `this` and `other`. 
    Warning: This does NOT merge their `perp`s. Code using this function should first extract `this->perp` 
    and `other->perp`, then merge them separately. */
    void merge(Direction* other, Predicate* pred);

    /* Identify pairs of angles `(a1, a2)` that need to be merged as a result of the directions `d` and 
    `other_d` being deduced as parallel. Additionally returns a `bool` which is true iff direction1
    is the one being coincident, and false if direction2 is the one being coincident.
    Also removes `a2` from `other_d->on_angles_1` and `other_d->on_angles_2`. */
    static Generator<std::pair<std::pair<Angle*, Angle*>, bool>> 
    check_incident_angles(Direction* d, Direction* other_d);

    static bool is_para(Direction* d1, Direction* d2);
    static bool is_perp(Direction* d1, Direction* d2);
};



/* Length class.

A Length indicates the length of some `Segment`.

Every Length stores the set of Ratios for which it is the numerator (length1) and denominator (length2)
respectively. These must always be root Ratios. Only root Lengths maintain these sets. */
class Length : public Value<Segment> {
public:
    std::set<Ratio*> on_ratio_1;
    std::set<Ratio*> on_ratio_2;
    Length(std::string name) : Value(name) {}

    /* Associate the segment `s` with the root length of `this`, by adding the former to the `objs` 
    and `root_objs` of the latter. 
    Calls `Segment::set_length()`, which does the exact same thing.
    Note: If `s` is already present in `objs`, then nothing happens. */
    void add_segment(Segment* s);

    Generator<std::pair<Segment*, Segment*>> all_cong_pairs();
    Generator<std::pair<Segment*, Segment*>> all_cong_pairs_ordered();

    /* Returns all root ratios for which the root of this length is a `length1` */
    Generator<Ratio*> on_ratios_as_length1();
    /* Returns all root ratios for which the root of this length is a `length2` */
    Generator<Ratio*> on_ratios_as_length2();

    /* Merges the root nodes of `this` and `other`. */
    void merge(Length* other, Predicate* pred);

    /* Identify pairs of ratios `(r1, r2)` that need to be merged as a result of the lengths `l` and `other_l`
    being deduced as equal. Additionally returns a `bool` which is true iff length1 is the one being coincident, 
    and false if length2 is the one being coincident.
    Also removes `r2` from `other_l->on_ratio_1` and `other_l->on_ratio_2`. */
    static Generator<std::pair<std::pair<Ratio*, Ratio*>, bool>>
    check_incident_ratios(Length* l, Length* other_l);

    /* Identify triples of points `(p1, p2, p3)`, where `p1p2` has length `l` and `p2p3` has length `other_l`. */
    static Generator<std::array<Point*, 3>> check_incident_isosceles_triangles(Length* l, Length* other_l);

    static bool is_cong(Length* l1, Length* l2);
};