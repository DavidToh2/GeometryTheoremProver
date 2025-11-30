#pragma once 

#include <map>
#include <memory>
#include <set>
#include <string>
#include <variant>
#include <vector>

#include "Numerics/Numerics.hh"
#include "Common/Generator.hh"
#include "Common/Constants.hh"
#include "Common/Utils.hh"

class Object;
class Point;
class Predicate;

class Arg {

public:
	std::variant<std::monostate, Object*, Frac, char> arg;

	Arg() {}
	Arg(Object* obj) : arg(obj) {}
	Arg(Frac f) : arg(f) {}
	Arg(char c) : arg(c) {}

	void clear();
	bool empty();
	bool filled();

	void set(Object* obj);
	void set(Frac f);
	void set(char c);

	/* Note: Returns `nullptr` if the argument stored is not an `Object*`.
	Performs a `static_cast` to `Point*` regardless of whether the `Object*` argument's true type is a `Point*`. */
	Point* get_point();

	static void populate_args_and_argmap(const std::string arg_str, std::vector<std::unique_ptr<Arg>> &arg_unique_ptr_vec, std::map<std::string, Arg*> &argmap);

	void operator=(Object* obj);
	void operator=(Frac f);
	void operator=(char c);

	std::string to_string();
};

class GeometricGraph;

class PredicateTemplate {
	std::string id;

public:
	pred_t name;
	std::vector<Arg*> args;
	
	PredicateTemplate(const pred_t name) : name(name) {};
	PredicateTemplate(const std::string name) : name(Utils::to_pred_t(name)) {};
	PredicateTemplate(const pred_t name, std::vector<Arg*> &args) : name(name), args(std::move(args)) {};
	PredicateTemplate(const std::string name, std::vector<Arg*> &args) : name(Utils::to_pred_t(name)), args(std::move(args)) {};
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

	bool __validate_neq(GeometricGraph &ggraph);
	bool __validate_ncoll(GeometricGraph &ggraph);
	bool validate_degeneracy_args(GeometricGraph &ggraph);
};

class Predicate {
public:
	std::string hash;

	pred_t name;
	std::vector<Object*> args;
	Frac frac_arg;
	std::set<Predicate*> why; // keep

	Predicate() : name(pred_t::BASE), hash("BASE") {};
	Predicate(const pred_t name, std::vector<Object*> &&objs);
	Predicate(const std::string pred_name, std::vector<Object*> &&objs);
	Predicate(PredicateTemplate &pred_template);

	static std::unique_ptr<Predicate> from_global_point_map(const std::string pred_string, std::map<std::string, std::unique_ptr<Point>> &global_point_map);

	std::string to_string();
};

class PredVec {
public:
	std::vector<Predicate*> preds;

	PredVec() {}

	void emplace_back(Predicate* pred);
	void operator+=(Predicate* pred);
	void operator+=(const PredVec& other);

	explicit operator std::vector<Predicate*>() const { return preds;}
};

class ClauseTemplate {

public:
	std::string name;
	std::vector<Arg*> args;
	std::vector<std::unique_ptr<PredicateTemplate>> predicates;

	ClauseTemplate() {}
	ClauseTemplate(std::string clause_string, std::map<std::string, Arg*> &argmap);

	void empty_args();
	bool is_empty();

	Generator<std::unique_ptr<Predicate>> instantiate();
	Generator<std::string> instantiate_hashes();

	std::string to_string();
};