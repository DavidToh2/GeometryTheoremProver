#pragma once

#include <map>
#include "Node.hh"

class Value;
class Measure;
class Length;
class Fraction;

class Object2;

class Value : Node {
    std::map<Object2*, Predicate*> obj2;
};

class Measure : Value {

};

class Length : Value {

};

class Fraction : Value {

};