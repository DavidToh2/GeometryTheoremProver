#pragma once

#include <map>
#include <set>
#include "Object.hh"

class Predicate;
class Predicate2;

class Node;
class Angle;
class Segment;
class Ratio;

class Value;

class Object2 : Node {
    std::map<Object*, Predicate*> obj;
    std::set<Predicate2*> pred2;

    Value* val;
};

class Angle : Object2 {

};

class Segment : Object2 {

};

class Ratio : Object2 {

};