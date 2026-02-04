#pragma once

#include <map>
#include <optional>

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
class Ratio;

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

## Maps to individual nodes for traceback

The `on_` and `_of_` maps store the objects that this point lies on, at the point at which it was placed onto
them, as well as its own identity at the point at which it happened. This is an immutable record that is not
changed regardless of future merges. This helps keep track of the exact relationships between nodes for later
traceback. 

When this point is later merged, the reasons for the merges are appended to the `why` s stored in these maps.
For example, if point `p1` is placed onto line `l` because of reason `pred1`, then `p1` is merged into `p2` 
with reason `pred2`, the record `p2->on_line[l][p1] = { pred1, pred2 }`.

When an object (say a line `l1`) is merged into another (say `l2`) with reason `pred`:
- every point `p` containing `l1` in `on_root_line` will replace it with `l2`; 
- for each such `p`, its `p->on_line[l1]` may contain multiple entries `{ p1 : preds1, ... }`;
- add `l2` to `on_line` so that `p->on_line[l2][pk] = { pred + preds1 }`.

This allows for a traceback procedure as such: if point `p` was initially placed on line `l`, then after a
series of merges the roots of `p` and `l` are `rp` and `rl`, then
```
    rp->on_line[rl][p] = (reasons for all the mergers) + p->on_line[l][p]
```

## Maps to root nodes for lookup

Only root nodes shall populate the `on_` maps: this is guaranteed by `merge_dmaps()` which is invoked by
`Point::merge()`.

The `on_root_` and `_of_root_` sets store the root objects that this point lies on.

Only root nodes shall populate the `on_root_` sets: this is guaranteed by `set::merge()` which is invoked by
`Point::merge()` (and leaves behind remnants in the child nodes).

After a merge, it is possible that some other `Object` s will now coincide and have to be merged. This logic
is handlded by `GeometricGraph::merge_points()`.
*/
class Point : public Node {
public:
    std::map<Line*, std::map<Point*, PredVec>> on_line;
    std::map<Circle*, std::map<Point*, PredVec>> on_circle;
    std::map<Circle*, std::map<Point*, PredVec>> center_of_circle;
    std::map<Segment*, std::map<Point*, PredVec>> endpoint_of_segment;
    std::set<Line*> on_root_line;
    std::set<Circle*> on_root_circle;
    std::set<Circle*> center_of_root_circle;
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
    /* Sets `this` point to be the center of circle `c`. What this does:
    - Inserts `c` into `this->center_of_circle` along with `pred`;
    - Inserts `root_c` into `this->center_of_root_circle`;
    - Inserts `this` into `c->center` along with `pred`.
    Note: Assumes that `this` is a root node.
    Note: This function is idempotent. */
    void set_this_center_of(Circle* c, Predicate* pred);
    /* Sets `this` point as an endpoint of segment `s`. What this does:
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
    PredVec __why_on(Line* l);
    PredVec why_on(Line* l);
    PredVec why_on(Circle* c);

    /* Returns the root line nodes that the root of this point is on */
    Generator<Line*> on_lines();
    /* Returns the root circle nodes that the root of this point is on*/
    Generator<Circle*> on_circles();
    /* Returns the root circle nodes for which the root of this point is the center */
    Generator<Circle*> center_of_circles();
    /* Returns the root segment nodes that the root of this point is an endpoint of */
    Generator<Segment*> endpoint_of_segments();
    Generator<std::pair<Segment*, Segment*>> endpoint_of_segment_pairs_ordered();

    /* Merge two point nodes. We merge them at their root nodes; it is pointless to merge anywhere else. 
    The `on_circle` and `on_line` of `get_root(other)` are moved into that of `get_root(this)`.
    All other `Objects` referencing `get_root(other)` are switched out to reference `get_root(this)`. 
    Note: This function has no effect if `this` and `other` already have the same root.*/
    void merge(Point* other, Predicate* pred);

    /* Merge two `on_` records in some `Point` object. This empties the second record. */
    template <std::derived_from<Object> Key>
    static void merge_dmaps(
        std::map<Key*, std::map<Point*, PredVec>> &dest, 
        std::map<Key*, std::map<Point*, PredVec>> &src, 
        Predicate* pred
    ) {
        for (auto it = src.begin(); it != src.end(); ) {
            Key* obj = it->first;
            if (!dest.contains(obj)) {
                dest[obj] = std::map<Point*, PredVec>();
            }
            // Note: dest[obj] and src[obj] should not have any overlapping keys, since the only possible
            // keys are the children of dest and src respectively
            for (auto jt = src[obj].begin(); jt != src[obj].end(); ++jt) {
                jt->second += pred;
            }
            dest[obj].merge(src[obj]);
            it = src.erase(it);
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
    Note: The directions of `root_other` and `root_this` are returned if they both exist. This is so they may 
    then be merged by `GeometricGraph::set_directions_para()`.
    Note: The copying behaviour does not copy over points that are already in `get_root(this)`. The effect is
    necessary because two lines being merged will necessarily have two duplicate points. 
    Note: This function has no effect if `this` and `other` already have the same root.*/
    std::optional<std::pair<Direction*, Direction*>> merge(Line* other, Predicate* pred);

    /* Identify all pairs of lines `(l1, l2)` that need to be merged (into a single line) as a result of `p` 
    and `other_p` being deduced to be the same. Any such pair of lines will have another point `q` lying on
    both. The tuples `{q, {l1, l2}}` are returned to then be checked for numeric equality - only when q is
    numerically distinct from both `p` and `other_p` do we merge the pair.
    Also replaces `other_p` with `p` in `l2->points`, and removes `l2` from `other_p->on_root_line`.
    Note: This function should be called *before* `p->merge(other_p)` occurs. */
    static Generator<std::pair<Point*, std::pair<Line*, Line*>>> 
    check_incident_lines(Point* p, Point* other_p, Predicate* pred);

    /* Identify all lines `l1` which might possibly be identical to both lines `l` and `other_l`, which are
    about to be merged. Any such line `l1` will have one point `p1` belonging to `l`, and another `p2` belonging
    to `other_l`. The tuples `{l1, {p1, p2}}` are returned to then be checked for numeric equality by the
    incidence detection algorithm.
    Note: This function should be called *before* `l->merge(other_l)` occurs.
    */
    static Generator<std::pair<Line*, std::pair<Point*, Point*>>> 
    check_incident_lines(Line* l, Line* other_l, Predicate* pred);
};



/* Circle class.

The Circle constructor should only ever be called with root points.

The `points` map is used for checking whether a point lies on the circle. `points` always stores root nodes; 
however, we only update `points` as long as the circle node is a root node. 

Circles can continue holding `points` even when they are themselves no longer roots (however these held `points`
will no longer serve any purpose). 

The circle `center` is self-explanatory, and need not necessarily be a root node. Always use `get_center()` to
obtain the `center` (which also lazily updates it to root). The most up-to-date `center` is always stored in
the root circle; however, get_center() can theoretically be called on any circle and will return the correct root
center. */
class Circle : public Object {
public:
    Point* center = nullptr;
    Predicate* center_why = nullptr;

    Circle(std::string name) : Object(name) {}
    Circle(std::string name, Point* p1, Point* p2, Point* p3, Predicate* base_pred) : Object(name) {
        points[p1] = base_pred;
        points[p2] = base_pred;
        points[p3] = base_pred;
    }
    Circle(std::string name, Point* c, Predicate* base_pred) : Object(name), center(c), center_why(base_pred) {

    }
    Circle(std::string name, Point* c, Point* p1, Predicate* base_pred) : Object(name), center(c), center_why(base_pred) {
        points[p1] = base_pred;
    }

    /* Sets the center of `this` circle to the root of `p`. 
    Note: If `this` circle already has a center, it is overwritten. */
    void __set_center(Point* p, Predicate* pred);
    /* Sets the center of the root node of `this` circle to the root of `p`.
    Note: If a center already exists, it is overwritten. */
    void set_center(Point* p, Predicate* pred);
    /* Checks if `this` circle has a center. */
    bool __has_center();
    /* Checks if the root of `this` circle has a center. */
    bool has_center();
    /* Gets the root node representing the center of `this` circle.
    Warning: throws if `this` circle does not have a center. */
    Point* __get_center();
    /* Gets the root node representing the center of the root of `this` circle.
    Warning: throws if the root of `this` circle does not have a center. */
    Point* get_center();

    Predicate* why_center();

    /* Returns all circles passing through the chord `p1p2`.
    This is done by looking at the intersection of the two `on_root_circle` sets of `p1` and `p2` respectively. 
    As a result, the generator is more of a design choice than an actual performance improvement. */
    static Generator<Circle*> all_circles_through(Point* p1, Point* p2);

    /* Merge two circle nodes which have been shown to be identical. We merge them at their root nodes. The 
    `points` of `get_root(other)` are copied into that of `get_root(this)`. 
    Note: The centers of `root_other` and `root_this` are returned if they both exist. This is so they may then
    be used by `GeometricGraph::merge_points()`.
    Note: The copying behaviour does not copy over points that are already in `get_root(this)`. The effect is
    necessary because two circles being merged will necessarily have two duplicate points. 
    Note: This function has no effect if `this` and `other` already have the same root.*/
    std::optional<std::pair<Point*, Point*>> merge(Circle* other, Predicate* pred);

    /* Identify all pairs of circles `(c1, c2)` that need to be merged (into a single circle) as a result of both 
    `p` and `other_p` being deduced to be the same. The circles in each pair either have exactly two common 
    intersections, with one additionally containing `p` and the other containing `other_p`; or they have a common
    center, with one having `p` as a point and the other having `other_p` as a point.
    Also replaces `other_p` with `p` in `c2->points`, and removes `c2` from `other_p->on_root_circle`.
    Note: This function should be called before `p->merge(other_p)` occurs. */
    static Generator<std::pair<std::pair<Point*, Point*>, std::pair<Circle*, Circle*>>> 
    check_incident_circles_by_intersections(Point* p, Point* other_p, Predicate* pred);

    /* Identify all pairs of circles `(c1, c2)` that need to be merged (into a single circle) as a result of both 
    `p` and `other_p` being deduced to be the same. The circles in each pair must have exactly one common point; 
    furthermore, one of the circles has `p` as its center, and the other has `other_p`.
    Also replaces `other_p` with `p` for `c2->center` (using `Circle::set_center()`).
    Note: This function should be called before `p->merge(other_p)` occurs. */
    static Generator<std::pair<Point*, std::pair<Circle*, Circle*>>> 
    check_incident_circles_by_center(Point* p, Point* other_p, Predicate* pred);
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
    /* Checks if segments `s1` and `s2` lie on the same line. */
    static bool on_same_line(Segment* s1, Segment* s2);

    /* Checks if the two endpoints of the segment are `p1, p2` or `p2, p1`.*/
    constexpr bool has_endpoints(Point* p1, Point* p2) const {
        return (
            (endpoints[0] == p1 && endpoints[1] == p2) ||
            (endpoints[0] == p2 && endpoints[1] == p1)
        );
    }
    /* Returns the other endpoint of the segment given one endpoint `p`. 
    Returns `nullptr` if `p` is not an endpoint of the segment. */
    constexpr Point* other_endpoint(Point* p) const { 
        return (
            (endpoints[0] == p) ? endpoints[1] : 
            ((endpoints[1] == p) ? endpoints[0] : nullptr)
        ); 
    } 

    Generator<Ratio*> on_ratios_as_segment1();
    Generator<Ratio*> on_ratios_as_segment2();

    /* Merge two segment nodes which have been shown to be identical. This only occurs when their endpoints
    have been shown to be identical. Only called by `Segment::check_incident_segments()`.
    Note: The lengths of `root_other` and `root_this` are returned if they both exist. This is so they may then
    be merged by `GeometricGraph::set_lengths_equal()`.
    Warning: Assumes that `this.endpoints == other.endpoints`. */
    std::optional<std::pair<Length*, Length*>> merge(Segment* other, Predicate* pred);

    /* Identify segments `(s1, s2)` that need to be merged as a result of the point `other_p` being merged 
    into the point `p`. This is done by checking those segments containing `p` and `other_p` as endpoints 
    respectively. 
    In case a segment has one endpoint `p` and another `other_p`, an error is thrown. 
    Also replaces occurences of `other_p` in `s2->endpoints` with `p`, and removes `s2` from 
    `other_p->endpoint_of_root_segment`.
    Note: Assumes that `p` and `other_p` are root points. */
    static Generator<std::pair<Segment*, Segment*>> check_incident_segments(Point* p, Point* other_p, Predicate* pred);
};



// TBA
class Triangle : public Object {
public:
    std::tuple<Point*, Point*, Point*> vertices;
    Shape* shape;
    
    Triangle(std::string name) : Object(name) {}


};

