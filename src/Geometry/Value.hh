#pragma once

#include <map>
#include "Object.hh"

class Object2;

class Value : public Node {
public:
    std::map<Object*, Predicate*> objs;
    Value(std::string name) : Node(name) {}
};

/* Direction class.

*/
class Direction : public Value {
public:

    Direction* perp;
    Direction(std::string name) : Value(name) {}

    void add_line(Line* l, Predicate* pred);
    void merge(Direction* other, Predicate* pred);
};

class Cong : public Value {
public:
    Cong(std::string name) : Value(name) {}
};

class Sim : public Value {
public:
    Sim(std::string name) : Value(name) {}
};