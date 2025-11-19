#pragma once

#include <map>
#include <vector>
#include "Node.hh"
#include <Numerics/Numerics.hh>

class Predicate;

class Point;
class Line;
class Circle;
class Triangle;
class Quadrilateral;

class Object : public Node {
    std::vector<std::pair<Point*, Predicate*>> points;

public:
    Object(std::string name) : Node(name) {}
    std::string to_string() { return name; }
};


class Point : public Object {
public:
    std::map<Circle*, Predicate*> on_circle;
    std::map<Line*, Predicate*> on_line;

    Point(std::string name) : Object(name) {}

    Coords* coords = nullptr;
};

class Line : public Object {
    
};

class Circle : public Object {
    std::pair<Point*, Predicate*> center;
};

class Triangle : public Object {

};

class Quadrilateral : public Object { 

};