#pragma once

#include <map>
#include <vector>

#include "Node.hh"
#include "Numerics/Numerics.hh"
#include "Common/Generator.hh"

class Predicate;
class PredVec;

class Point;
class Line;
class Circle;
class Triangle;

class Direction;
class Shape;

/* Object class.

`points` always stores root points. */
class Object : public Node {

public:
    std::map<Point*, Predicate*> points;
    Object(std::string name) : Node(name) {}

    Generator<Point*> all_points();
};



/* Point class.

`on_circle` and `on_line` need not store root circles and line nodes. As a result, multiple `Object` keys may 
exist in these maps with the same root.
They are read by predicate unification purposes by the DDEngine, as well as for traceback purposes, and are 
only written to when Point nodes are merged. 

`on_root_circle` and `on_root_line` store the root circles and lines that this point lies on.
They are written to when other `Object` nodes are merged.

Only root nodes populate their `on_` maps and `on_root_` sets.
*/
class Point : public Node {
public:
    std::map<Line*, std::map<Point*, PredVec>> on_line;
    std::map<Circle*, std::map<Point*, PredVec>> on_circle;
    std::set<Line*> on_root_line;
    std::set<Circle*> on_root_circle;

    Point(std::string name) : Node(name) {}

    Coords* coords = nullptr;

    /* Set `this` point to be on the line `l`. What this does:
    - Inserts `l` into `this->on_line` along with `pred`;
    - Inserts `root_l` into `this->on_root_line`;
    - Inserts `this` into `l->points` along with `pred`.
    
    Note: Assumes that `this` is a root node. */
    void set_this_on(Line* l, Predicate* pred);
    void set_this_on(Circle* c, Predicate* pred);
    /* Checks if `this` point lies on the root of node `l`. This is done by checking against the 
    set `on_root_line` of `this`.
    
    Note: Assumes that `this` is a root node. */
    bool is_this_on(Line* l);
    bool is_this_on(Circle* c);

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

    /* Returns the root line nodes that this point is on */
    Generator<Line*> on_lines();
    /* Returns the root circle nodes that this point is on*/
    Generator<Circle*> on_circles();

    /* Merge two point nodes. We merge them at their root nodes; it is pointless to merge anywhere else. 
    The `on_circle` and `on_line` of `get_root(other)` are moved into that of `get_root(this)`.
    All other `Objects` referencing `get_root(other)` are switched out to reference `get_root(this)`. */
    void merge(Point* other, Predicate* pred);

    /* Merge two `on_` records in some `Point` object. This empties the second record. 
    
    Note: Duplicate `Key*`s may remain in the `dest` after merge, in the sense that there may be two or 
    more `Key*`s with the same root. */
    template <std::derived_from<Object> Key>
    static void merge_dmaps(
        std::map<Key*, std::map<Point*, PredVec>> &dest, 
        std::map<Key*, std::map<Point*, PredVec>> &src, 
        Predicate* pred) 
    {
        for (const auto& [obj, _] : src) {
            if (!Utils::isinmap(obj, dest)) {
                dest[obj] = std::map<Point*, PredVec>();
            }
            for (const auto& [pt, _] : src[obj]) {
                src[obj][pt] += pred;
            }
            dest[obj].merge(src[obj]);
            src.erase(obj);
        }
    }
};



/* Line class.

The `points` map is used for checking whether a point lies on the line. As described before, `points` always
stores root points; however, we only update `points` as long as the line node is a root node. */
class Line : public Object {

public:
    Direction* direction = nullptr;
    Predicate* direction_why = nullptr;
    Line(std::string name) : Object(name) {}
    Line(std::string name, Point* p1, Point* p2, Predicate* base_pred) : Object(name) {
        points[p1] = base_pred;
        points[p2] = base_pred;
    }
    /* Checks if `this` contains the root node of `p`
    
    Note: assumes that `this` is a root node */
    bool __contains(Point *p);
    /* Checks if the root node of `this` contains the root node of `p` */
    bool contains(Point *p);

    /* Add the root node of `d` as the direction of the root node of `this`. 
    
    Note: If the root node of `this` already has a direction, overwriting occurs. Code using this function should
    manually check if `this` already has a direction. */
    void set_direction(Direction* d, Predicate* base_pred);

    /* Gets the direction node of this line. */
    Direction* __get_direction();
    /* Gets the root direction node of this line. */
    Direction* get_direction();
    

    /* Checks if `this` has a direction
    
    Note: assumes that `this` is a root node */
    bool __has_direction();
    /* Checks if the root node of `this` has a direction */
    bool has_direction();

    static bool is_para(Line* l1, Line* l2);
    static bool is_perp(Line* l1, Line* l2);

    /* Returns all pairs of root points on this line */
    Generator<std::pair<Point*, Point*>> all_point_pairs();
    /* Returns all triples of root points on this line */
    Generator<std::tuple<Point*, Point*, Point*>> all_point_triples();

    /* Merge two line nodes. We merge them at their root nodes. The `points` of `get_root(other)` are copied 
    into that of `get_root(this)`. 
    
    Note: The copying behaviour does not copy over points that are already in `get_root(this)`. The effect is
    necessary because two lines being merged will necessarily have two duplicate points. */
    void merge(Line* other, Predicate* pred);
};




class Circle : public Object {
public:
    std::pair<Point*, Predicate*> center;

    Circle(std::string name) : Object(name) {}

    bool contains(Point *p);
};

class Triangle : public Object {
public:
    std::tuple<Point*, Point*, Point*> vertices;
    Shape* shape;
    
    Triangle(std::string name) : Object(name) {}


};

