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
class Quadrilateral;

class Direction;
class Cong;
class Sim;

/* Object class.

`points` always stores root points. */
class Object : public Node {

public:
    std::map<Point*, Predicate*> points;
    Object(std::string name) : Node(name) {}
    std::string to_string() { return name; }
};



/* Point class.

`on_circle` and `on_line` need not store root circles and line nodes. They are used only for traceback 
purposes, and are only updated when Point nodes are merged. 
Only root nodes have non-empty `on_circle` and `on_line` s.

Note: The same line may appear multiple times, as multiple line nodes `l1, l2`, in some point's `on_line`
map. In fact, this is the primary way used to deduce that two points are the same (i.e. the `eq` predicate).

Invariant: The same point node `p` will never be added */
class Point : public Object {
public:
    std::map<Circle*, std::map<Point*, PredVec>> on_circle;
    std::map<Line*, std::map<Point*, PredVec>> on_line;

    Point(std::string name) : Object(name) {}

    Coords* coords = nullptr;

    void set_this_on(Circle* c, Predicate* pred);
    void set_this_on(Line* l, Predicate* pred);
    bool is_this_on(Line* l);
    bool is_this_on(Circle* c);

    void set_on(Circle* c, Predicate* pred);
    void set_on(Line* l, Predicate* pred);
    bool is_on(Line* l);
    bool is_on(Circle* c);

    Generator<Line*> on_lines();
    Generator<Circle*> on_circles();

    /* Merge two point nodes. We merge them at their root nodes; it is pointless to merge anywhere else. 
    The `on_circle` and `on_line` of `get_root(other)` are moved into that of `get_root(this)`.
    All other `Objects` referencing `get_root(other)` are switched out to reference `get_root(this)`. */
    void merge(Point* other, Predicate* pred);

    /* Merge two `on_` records in some `Point` object. This destroys the second record. */
    template <std::derived_from<Object> Key>
    static void merge_dmaps(
        std::map<Key*, std::map<Point*, PredVec>> &dest, 
        std::map<Key*, std::map<Point*, PredVec>> &src, 
        Predicate* pred
    );
};



/* Line class.

The `points` map is used for checking whether a point lies on the line. As described before, `points` always
stores root points; however, we only update `points` as long as the line node is a root node. */
class Line : public Object {

public:
    Direction* direction = nullptr;
    Line(std::string name) : Object(name) {}
    Line(std::string name, Point* p1, Point* p2, Predicate* base_pred) : Object(name) {
        points[p1] = base_pred;
        points[p2] = base_pred;
    }
    bool contains(Point *p);
    bool has_direction();

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

};

class Quadrilateral : public Object { 

};

