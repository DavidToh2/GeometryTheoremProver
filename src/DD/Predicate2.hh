#pragma once

#include <string>

#include "Predicate.hh"
#include <Geometry/Object2.hh>

class Predicate2 {
	std::string id;

	std::string name;
	std::string rule_name;
	std::vector<Arg> args;

	std::set<Predicate*> why;
	std::set<Predicate*> hence;

	std::set<Object2*> obj2;
};