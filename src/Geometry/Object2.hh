#pragma once

#include "Object.hh"
#include "Value.hh"

class Predicate;
class Predicate2;

class Node;
class Angle;
class Ratio;

class Measure;
class Length;
class Fraction;

/* Object2 class.

Object2's will generally always relate to some Objects. Only root Objects should ever be stored
as members of Object2's. Analogously, only root Object2's should ever be stored as members of
Objects. */
class Object2 : public Node {
public:
    Object2(std::string name) : Node(name) {}
};


/* Angle class. 

Every Angle stores a pair of `direction` s. These `direction` s must always be root nodes. The most up-to-date
copies are always stored in the root node.

Every angle also stores a `measure (Value2)`. The `measure` need not necessarily be a root node. Always use 
the `get_measure()` method to obtain the `measure` (which also lazily updates it to root). */
class Angle : public Object2 {
public:
    Direction* direction1;
    Direction* direction2;
    Measure* measure = nullptr;
    Predicate* measure_why = nullptr;

    Angle(std::string name, Direction* d1, Direction* d2) : Object2(name), direction1(d1), direction2(d2) {}
    
    /* Adds the root node of `m` as the measure of the root node of `this`.
    This updates the `obj2s` and `root_obj2s` of `root_m`, as well as the `measure` and `measure_why` of `root_this`.
    Warning: If the root node of `this` already has a measure, the new measure is merged into the old.
    Warning: Code using this function should manually check if `this` already has a measure. */
    void set_measure(Measure* m, Predicate* base_pred);
    /* Checks if `this` has a measure.
    Note: assumes that `this` is a root node */
    bool __has_measure();
    /* Checks if the root node of `this` has a measure. */
    bool has_measure();
    /* Gets the root measure node of this angle.
    Note: Assumes that the angle already has a measure.
    Note: This function also lazily updates `measure` to the root `Measure` node. */
    Measure* __get_measure();
    /* Gets the root measure node of the root of this angle.
    Note: Assumes that the root angle already has a measure.
    Note: This function also lazily updates `measure` to the root `Measure` node. */
    Measure* get_measure();

    /* Checks if angles `a1` and `a2` are equal. 
    This is done by fetching the roots of `a1` and `a2`, then checking if their root measures are the same. */
    static bool is_equal(Angle* a1, Angle* a2);
    /* Checks the root of angle `a`'s root measure's `val` attribute to see if it is equal to `f`. */
    static bool is_equal(Angle* a, Frac f);

    Generator<std::pair<Line*, Line*>> all_line_pairs();

    /* Merges `other` into `this`. 
    Note: `this` and `other` should be root nodes.
    WARNING: This assumes that the `direction1` and `direction2` of `this` and `other` are already equal. The
    function has undefined behaviour if they are not. */
    void __merge(Angle* other, Predicate* pred);
    /* Merges the root node of `other` into the root node of `this`.
    This should only be done after the `direction1` and `direction2` of both angles have been merged.
    Note: Throws if `direction1` and `direction2` are not equal. */
    void merge(Angle* other, Predicate* pred);
};

class Ratio : public Object2 {
public:
    Length* length1;
    Length* length2;
    Fraction* fraction = nullptr;
    Predicate* fraction_why = nullptr;

    Ratio(std::string name, Length* l1, Length* l2) : Object2(name), length1(l1), length2(l2) {}

    /* Adds the root node of `f` as the fraction of the root node of `this`.
    This updates the `obj2s` and `root_obj2s` of `root_f`, as well as the `fraction` and `fraction_why` of `root_this`.
    Warning: If the root node of `this` already has a fraction, the new fraction is merged into the old.
    Warning: Code using this function should manually check if `this` already has a fraction. */ 
    void set_fraction(Fraction* f, Predicate* base_pred);
    /* Checks if `this` has a fraction.
    Note: assumes that `this` is a root node */
    bool __has_fraction();
    /* Checks if the root node of `this` has a fraction. */
    bool has_fraction();
    /* Gets the root fraction node of this ratio.
    Note: Assumes that this ratio already has a fraction. 
    Note: This function also lazily updates `fraction` to the root `Fraction` node. */
    Fraction* __get_fraction();
    /* Gets the root fraction node of the root of this ratio.
    Note: Assumes that the root ratio already has a fraction. 
    Note: This function also lazily updates `fraction` to the root `Fraction` node. */
    Fraction* get_fraction();

    /* Checks if ratios `r1` and `r2` are equal.
    This is done by fetching the roots of `r1` and `r2`, then checking if their root fractions are the same. */
    static bool is_equal(Ratio* r1, Ratio* r2);
    /* Checks the root of ratio `r`'s root fraction's `val` attribute to see if it is equal to `f`. */
    static bool is_equal(Ratio* r, Frac f);

    Generator<std::pair<Segment*, Segment*>> all_segment_pairs();

    /* Merges `other` into `this`. 
    Note: `this` and `other` should be root nodes.
    WARNING: This assumes that the `length1` and `length2` of `this` and `other` are already equal. The function 
    has undefined behaviour if they are not. */
    void __merge(Ratio* other, Predicate* pred);
    /* Merges the root node of `other` into the root node of `this`.
    This should only be done after the `length1` and `length2` of both ratios have been merged.
    Note: Throws if `length1` and `length2` are not equal. */
    void merge(Ratio* other, Predicate* pred);
};