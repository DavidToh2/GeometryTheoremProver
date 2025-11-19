#pragma once 

#include <map>
#include <memory>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include "Geometry/Object.hh"
#include "Numerics/Numerics.hh"
#include "Common/Generator.hh"

class Arg {

public:
	std::variant<std::monostate, Object*, Frac, char> arg;

	Arg() {}
	Arg(Object* obj) : arg(obj) {}
	Arg(Frac f) : arg(f) {}
	Arg(char c) : arg(c) {}

	void clear();
	bool empty();

	void set(Object* obj);
	void set(Frac f);
	void set(char c);

	std::string to_string();

	static void populate_args_and_argmap(const std::string arg_str, std::vector<std::unique_ptr<Arg>> &arg_unique_ptr_vec, std::map<std::string, Arg*> &argmap);

	void operator=(Object* obj);
	void operator=(Frac f);
	void operator=(char c);
};

class PredicateTemplate {
	std::string id;

public:
	std::string name;
	std::vector<Arg*> args;
	
	PredicateTemplate(const std::string name) : name(name) {}
	PredicateTemplate(const std::string name, std::vector<Arg*> &args) : name(name), args(std::move(args)) {};
	PredicateTemplate(const std::string pred_string, std::map<std::string, Arg*> &argmap);

	void set_arg(int i, Object* obj) noexcept;
	void set_arg(int i, Frac f) noexcept;
	void set_arg(int i, char c) noexcept;
	void clear_arg(int i) noexcept;

	void set_args(std::vector<Object*> objs);
	void set_args(std::vector<Object*> objs, Frac f);
	void clear_args();

	std::unique_ptr<Predicate> instantiate();

	std::string to_string();
	std::string to_hash_with_args();
};

class Predicate {
public:
	std::string hash;
	std::vector<Object*> args;
	Frac frac_arg;
	std::set<Predicate*> why; // keep

	Predicate(const std::string pred_name, std::vector<Object*> &&objs);
	Predicate(PredicateTemplate &pred_template);

	static std::unique_ptr<Predicate> from_global_point_map(const std::string pred_string, std::map<std::string, std::unique_ptr<Point>> &global_point_map);

	std::string to_string();
};

class Clause {

public:
	std::string name;
	std::vector<Arg*> args;
	std::vector<std::unique_ptr<PredicateTemplate>> predicates;

	Clause() {}
	Clause(std::string clause_string, std::map<std::string, Arg*> &argmap);

	void empty_args();
	bool is_empty();

	Generator<std::unique_ptr<Predicate>> instantiate();
	Generator<std::string> instantiate_hashes();

	std::string to_string();
};