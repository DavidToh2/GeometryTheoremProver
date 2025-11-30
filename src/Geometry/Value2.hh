#pragma once

#include <map>
#include "Node.hh"

class Value;
class Measure;
class Length;
class Fraction;

class Object2;

class Value2 : Node {
    std::map<Object2*, Predicate*> obj2;
};

class Measure : Value2 {

};

class Length : Value2 {

};

class Fraction : Value2 {

};