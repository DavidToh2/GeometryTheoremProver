#pragma once

#include <map>

#include "Node.hh"
#include "Common/Generator.hh"

class Predicate;
class PredVec;

class Point;
class Line;
class Circle;
class Segment;
class Triangle;

class Direction;
class Length;
class Shape;

class Angle;

/* Object class.

`points` always stores root points. The most up-to-date copies of `points are stored
by root objects. */
class Object : public Node {

public:
    std::map<Point*, Predicate*> points;
    Object(std::string name) : Node(name) {}

    /* Returns every root point on the root of this object */
    Generator<Point*> all_points();
    /* Returns all pairs of root points on the root of this object */
    Generator<std::pair<Point*, Point*>> all_point_pairs();
    /* Returns all ordered pairs of root points on the root of this object */
    Generator<std::pair<Point*, Point*>> all_point_pairs_ordered();
    /* Returns all triples of root points on the root of this object */
    Generator<std::tuple<Point*, Point*, Point*>> all_point_triples();
    /* Returns all ordered triples of root points on the root of this object */
    Generator<std::tuple<Point*, Point*, Point*>> all_point_triples_ordered();
    /* Returns all quadruples of root points on the root of this object */
    Generator<std::tuple<Point*, Point*, Point*, Point*>> all_point_quads();
    /* Returns all ordered quadruples of root points on the root of this object */
    Generator<std::tuple<Point*, Point*, Point*, Point*>> all_point_quads_ordered();

    /* Checks if `this` contains the root node of `p`
    Note: assumes that `this` is a root node */
    bool __contains(Point *p);
    /* Checks if the root node of `this` contains the root node of `p` */
    bool contains(Point *p);

    /* Gets the predicate explaining why `root_p` lies on `root_this`
    Note: assumes that `this` is a root node */
    Predicate* __why_contains(Point* p);
    /* Gets the predicate explaining why `root_p` lies on `root_this` */
    Predicate* why_contains(Point *p);
};



/* Point class.

`on_circle` and `on_line` need not store root circles and line nodes. As a result, multiple `Object` keys may 
exist in these maps with the same root.
They are only read for traceback purposes, and are only written to when Point nodes are merged (using the
`Point::merge_dmaps` function).

`on_root_circle` and `on_root_line` store the root circles and lines that this point lies on. They are written 
to when other `Object` nodes are merged.
Only root point nodes populate their `on_` maps and `on_root_` sets. All other points have empty `on_` maps and 
`on_root_` sets.
*/
class Point : public Node {
public:
    std::map<Line*, std::map<Point*, Predicate*>> on_line;
    std::map<Circle*, std::map<Point*, Predicate*>> on_circle;
    std::map<Segment*, std::map<Point*, Predicate*>> endpoint_of_segment;
    std::set<Line*> on_root_line;
    std::set<Circle*> on_root_circle;
    std::set<Segment*> endpoint_of_root_segment;

    Point(std::string name) : Node(name) {}

    /* Set `this` point to be on the line `l`. What this does:
    - Inserts `l` into `this->on_line` along with `pred`;
    - Inserts `root_l` into `this->on_root_line`;
    - Inserts `this` into `l->points` along with `pred`.
    Note: Assumes that `this` is a root node.
    Note: This function is idempotent. */
    void set_this_on(Line* l, Predicate* pred);
    /* Set `this` point to be on the circle `c`. What this does:
    - Inserts `c` into `this->on_circle` along with `pred`;
    - Inserts `root_c` into `this->on_root_circle`;
    - Inserts `this` into `c->points` along with `pred`.
    Note: Assumes that `this` is a root node.
    Note: This function is idempotent. */
    void set_this_on(Circle* c, Predicate* pred);
    /* Sets `this` point as an endpoint of segment `s`. This is called by the Segment constructor,
    and so should NOT be invoked when creating segments. What this does:
    - Inserts `s` into `this->endpoint_of_segment` along with `pred`;
    - Inserts `root_s` into `this->endpoint_of_root_segment`;
    Note: Assumes that `this` is a root node.
    Note: This function is idempotent. */
    void set_this_endpoint_of(Segment* s, Predicate* pred);

    /* Checks if `this` point lies on the root of node `l`. This is done by checking against the 
    set `on_root_line` of `this`.
    Note: Assumes that `this` is a root node. */
    bool is_this_on(Line* l);
    bool is_this_on(Circle* c);
    bool is_this_endpoint_of(Segment* s);

    /* Set `root_this` to be on the line `l`. What this does:
    - Inserts `l` into `root_this->on_line` along with `pred`;
    - Inserts `root_l` into `root_this->on_root_line`;
    - Inserts `root_this` into `l->points` along with `pred`. */
    void set_on(Line* l, Predicate* pred);
    void set_on(Circle* c, Predicate* pred);
    /* Checks if `root_this` lies on the root of node `l`. This is done by checking against the 
    set `on_root_line` of `root_this`. */
    bool is_on(Line* l);
    bool is_on(Circle* c);

    /* Gets the predicate `on_line[l][this]` as stored in `root_this`. */
    Predicate* __why_on(Line* l);
    Predicate* why_on(Line* l);
    Predicate* why_on(Circle* c);

    /* Returns the root line nodes that this point is on */
    Generator<Line*> on_lines();
    /* Returns the root circle nodes that this point is on*/
    Generator<Circle*> on_circles();

    /* Merge two point nodes. We merge them at their root nodes; it is pointless to merge anywhere else. 
    The `on_circle` and `on_line` of `get_root(other)` are moved into that of `get_root(this)`.
    All other `Objects` referencing `get_root(other)` are switched out to reference `get_root(this)`. 
    Note: This function has no effect if `this` and `other` already have the same root.*/
    void merge(Point* other, Predicate* pred);

    /* Merge two `on_` records in some `Point` object. This empties the second record. */
    template <std::derived_from<Object> Key>
    static void merge_dmaps(
        std::map<Key*, std::map<Point*, Predicate*>> &dest, 
        std::map<Key*, std::map<Point*, Predicate*>> &src
    ) {
        for (const auto& [obj, _] : src) {
            if (!Utils::isinmap(obj, dest)) {
                dest[obj] = std::map<Point*, Predicate*>();
            }
            // Note: dest[obj] and src[obj] should not have any overlapping keys, since the only possible
            // keys are the children of dest and src respectively
            dest[obj].merge(src[obj]);
            src.erase(obj);
        }
    }
};



/* Line class.

The Line constructor should only ever be called with root points.

The `points` map is used for checking whether a point lies on the line. `points` always stores root nodes; 
however, we only update `points` as long as the line node is a root node. 

Lines can continue holding `points` even when they are themselves no longer roots (however these held `points`
will no longer serve any purpose). 

The `on_angles_1` and `on_angles_2` sets store the root angles for which this line is the `line1` and `line2`
respectively. Only root lines should have these sets populated.

Every line also stores a `direction (Value)`. The `direction` need not necessarily be a root node. Always use 
the `get_direction()` method to obtain the `direction` (which also lazily updates it to root). */
class Line : public Object {

public:
    Direction* direction = nullptr;
    Predicate* direction_why = nullptr;

    Line(std::string name) : Object(name) {}
    Line(std::string name, Point* p1, Point* p2, Predicate* base_pred) : Object(name) {
        points[p1] = base_pred;
        points[p2] = base_pred;
        p1->set_this_on(this, base_pred);
        p2->set_this_on(this, base_pred);
    }

    /* Add the root node of `d` as the direction of the root node of `this`. 
    This updates the `objs` and `root_objs` of `root_d`, as well as the `direction` and `direction_why` of `root_this`.
    Warning: If the root node of `this` already has another direction, the directions are merged.
    Warning: Code using this function should manually check if `this` already has a direction. */
    void set_direction(Direction* d, Predicate* pred);
    /* Checks if `this` has a direction.
    Note: assumes that `this` is a root node. */
    bool __has_direction();
    /* Checks if the root node of `this` has a direction. */
    bool has_direction();
    /* Gets the direction node of this line.
    Note: This function also lazily updates `direction` to the root `Direction` node.
    Warning: Throws if `this` does not have a direction. */
    Direction* __get_direction();
    /* Gets the root direction node of this line.
    Note: This function also lazily updates `direction` to the root `Direction` node.
    Warning: Throws if `this` does not have a direction. */
    Direction* get_direction();

    Predicate* why_direction();

    /* Checks if `l1` and `l2` are parallel.
    This is done by fetching the roots of `l1` and `l2`, then checking if their root directions are the same.
    Note: Returns `true` in the edge case where `l1 == l2`. */
    static bool is_para(Line* l1, Line* l2);
    /* Checks if `l1` and `l2` are perpendicular.
    This is done by fetching the roots of `l1` and `l2`, then checking if the root direction of `l1` has 
    a `get_perp()` that is equal to the root direction of `l2`. */
    static bool is_perp(Line* l1, Line* l2);

    /* Returns all root angles for which the root of this line is a `line1` */
    Generator<Angle*> on_angles_as_line1();
    /* Returns all root angles for which the root of this line is a `line2` */
    Generator<Angle*> on_angles_as_line2();

    /* Merge two line nodes. We merge them at their root nodes. The `points` of `get_root(other)` are copied 
    into that of `get_root(this)`. 
    Note: The copying behaviour does not copy over points that are already in `get_root(this)`. The effect is
    necessary because two lines being merged will necessarily have two duplicate points. 
    Note: This function has no effect if `this` and `other` already have the same root.*/
    void merge(Line* other, Predicate* pred);
};



/* Circle class.

The Circle constructor should only ever be called with root points.

The `points` map is used for checking whether a point lies on the circle. `points` always stores root nodes; 
however, we only update `points` as long as the circle node is a root node. 

Circles can continue holding `points` even when they are themselves no longer roots (however these held `points`
will no longer serve any purpose). 

The circle `center` is self-explanatory, and need not necessarily be a root node. Always use `get_center()` to
obtain the `center` (which also lazily updates it to root). */
class Circle : public Object {
public:
    Point* center = nullptr;
    Predicate* center_why = nullptr;

    Circle(std::string name) : Object(name) {}
    Circle(std::string name, Point* p1, Point* p2, Point* p3, Predicate* base_pred) : Object(name) {
        points[p1] = base_pred;
        points[p2] = base_pred;
        points[p3] = base_pred;
        p1->set_this_on(this, base_pred);
        p2->set_this_on(this, base_pred);
        p3->set_this_on(this, base_pred);
    }
    Circle(std::string name, Point* c, Predicate* base_pred) : Object(name), center(c), center_why(base_pred) {}
    Circle(std::string name, Point* c, Point* p1, Predicate* base_pred) : Object(name), center(c), center_why(base_pred) {
        points[p1] = base_pred;
        p1->set_this_on(this, base_pred);
    }

    /* Sets the center of `this` circle to the root of `p`. 
    Note: If `this` circle already has a center, then it is merged into the root of `p`. */
    void __set_center(Point* p, Predicate* pred);
    /* Sets the center of the root node of `this` circle to the root of `p`.
    Note: If a center already exists, then it is merged into the root of `p`. */
    void set_center(Point* p, Predicate* pred);
    /* Gets the root node representing the center of `this` circle. */
    Point* __get_center();
    /* Gets the root node representing the center of the root of `this` circle. */
    Point* get_center();

    Predicate* why_center();

    /* Returns all circles passing through the chord `p1p2`.
    This is done by looking at the intersection of the two `on_root_circle` sets of `p1` and `p2` respectively. 
    As a result, the generator is more of a design choice than an actual performance improvement. */
    static Generator<Circle*> all_circles_through(Point* p1, Point* p2);

    /* Merge two circle nodes which have been shown to be identical. We merge them at their root nodes. The 
    `points` of `get_root(other)` are copied into that of `get_root(this)`. The center of `root_other` is merged 
    into that of `root_this`.
    Note: The copying behaviour does not copy over points that are already in `get_root(this)`. The effect is
    necessary because two circles being merged will necessarily have two duplicate points. 
    Note: This function has no effect if `this` and `other` already have the same root.*/
    void merge(Circle* other, Predicate* pred);
};



/* Segment class.

The Segment constructor should only ever be called with root points.

The `points` map stores all points on the segment. (I'm not sure what this will be useful for.)

The `endpoints` stored by the root segments will always be root nodes. These are kept up-to-date by the
`Point::merge()` function, which calls `Segment::check_segments_with_endpoint()` to merge the affected segments.
Note that segments are always created with their endpoints obeying the ordering of the CartesianPoint comparator - 
in other words, in GeometricGraph, we will always have `point_nums[p1] < point_nums[p2]` for a segment `p1p2`.

The `on_line` pointer stores the line that this segment lies on. This need not necessarily be a root node. Always
use `get_line()` which lazily updates it to the root node. 

Every segment always stores a `length (Value)`. This need not necessarily be a root node. Always use 
`get_length()`, which also lazily updates it to the root value. */
class Segment : public Object {
public:
    std::array<Point*, 2> endpoints;
    Length* length = nullptr;
    Predicate* length_why = nullptr;
    Line* on_line = nullptr;

    Segment(std::string name) : Object(name) {}
    Segment(std::string name, Point* p1, Point* p2, Line* l, Predicate* base_pred) : Object(name), endpoints({p1, p2}), on_line(l) {
        points[p1] = base_pred;
        points[p2] = base_pred;
        p1->set_this_endpoint_of(this, base_pred);
        p2->set_this_endpoint_of(this, base_pred);
    }

    /* Sets the length of the root node of `this` segment to the root of `l`.
    Warning: If the root node of `this` already has another length, the new length is merged into the old.
    Warning: Code using this function should manually check if `this` already has a length.  */
    void set_length(Length* l, Predicate* pred);
    /* Checks if `this` has a length.
    Note: assumes that `this` is a root node. */
    bool __has_length();
    /* Checks if the root node of `this` has a length. */
    bool has_length();
    /* Gets the root node representing the length of this segment.
    Note: `this->length` is lazily updated to the root node by this function. */
    Length* __get_length();
    /* Gets the root node representing the length of the root of `this` segment. */
    Length* get_length();

    /* Gets the root node representing the line that this segment lies on.
    Note: `this->on_line` is lazily updated to the root node by this function. */
    Line* __get_line();
    /* Gets the root node representing the line that the root of `this` segment lies on. */
    Line* get_line();

    constexpr Point* other_endpoint(Point* p) { return (endpoints[0] == p) ? endpoints[1] : endpoints[0]; } 

    /* Merge two segment nodes which have been shown to be identical. This only occurs when their endpoints
    have been shown to be identical. Only called by `check_segments_with_endpoint()`. */
    void merge(Segment* other, Predicate* pred);

    /* Called by `p.merge(other_p)`. 
    This function checks the second endpoints of those segments starting with `p`, as well as the endpoints of 
    those segments starting with `other_p`, and performs merges of segment pairs whose second endpoints coincide.
    In case a segment has one endpoint `p` and another `other_p`, an error is thrown. 
    Note: Assumes that `p` and `other_p` are root points. */
    static void check_segments_with_endpoint(Point* p, Point* other_p, Predicate* pred);
};



// TBA
class Triangle : public Object {
public:
    std::tuple<Point*, Point*, Point*> vertices;
    Shape* shape;
    
    Triangle(std::string name) : Object(name) {}


};

