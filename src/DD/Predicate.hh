#pragma once 

#include <map>
#include <memory>
#include <variant>
#include <set>
#include <string>
#include <vector>

#include "Numerics/Numerics.hh"
#include "Common/Generator.hh"
#include "Common/Constants.hh"
#include "Common/Utils.hh"

class Node;
class Object;
class Point;
class Predicate;

/* Arg class.

Arguments may be set using the `set()` method. This returns a `char` which may take one of three values depending on
the following:

- Unsuccessful set, returns 0: if the argument was already set, is being set to a `Node*`, and is different to its
previous value
- Successful set, returns 1: if the argument was previously empty and is now set
- Unchanged set, returns 2: if the argument was already set to the same `Node*`
*/
class Arg {

public:
	const static char UNSUCCESSFUL_SET = 0;
	const static char SUCCESSFUL_SET = 1;
	const static char UNCHANGED_SET = 2;
	std::variant<std::monostate, Node*, Frac, char> arg;

	Arg() {}
	Arg(Node* node) : arg(node) {}
	Arg(Frac f) : arg(f) {}
	Arg(char c) : arg(c) {}

	void clear();
	bool empty();
	bool filled();

	char set(Node* node);
	char set(Frac f);
	char set(char c);

	/* Note: Returns `nullptr` if the argument stored is not an `Node*`.
	Performs a `static_cast` to `Point*` regardless of whether the `Node*` argument's true type is a `Point*`. */
	Point* get_point();

	static void populate_args_and_argmap(
		const std::string arg_str, 
		std::vector<std::unique_ptr<Arg>> &arg_unique_ptr_vec, 
		std::map<std::string, Arg*> &argmap);

	void operator=(Node* node);
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
	// This constructor literally only exists to convert the conclusion from a Predicate to a PredicateTemplate
	PredicateTemplate(Predicate* pred, std::vector<std::unique_ptr<Arg>> &arglist);

	/* See the `Arg` class documentation for more details about the return values. In short:
	- Unsuccessful set, returns 0, if the argument was already set to a different `Node*` 
	- Successful set, returns 1: if the argument was previously empty
	- Unchanged set, returns 2: if the argument was already set to the same `Node*`*/
	char set_arg(int i, Node* node) noexcept;
	char set_arg(int i, Frac f) noexcept;
	char set_arg(int i, char c) noexcept;
	bool arg_empty(int i) noexcept;
	void clear_arg(int i) noexcept;

	/* Note: Returns 0 or 1 depending on whether there were any unsuccessful sets */
	char set_args(std::vector<Node*> nodes);
	/* Note: Returns 0 or 1 depending on whether there were any unsuccessful sets */
	char set_args(std::vector<Node*> nodes, Frac f);
	void clear_args();

	Point* get_arg_point(int i);

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
	std::vector<Node*> args;
	Frac frac_arg;
	std::set<Predicate*> why; // keep

	Predicate() : name(pred_t::BASE), hash("BASE") {};
	Predicate(const pred_t name, std::vector<Node*> &&nodes);
	Predicate(const std::string pred_name, std::vector<Node*> &&nodes);
	Predicate(PredicateTemplate &pred_template);

	static std::unique_ptr<Predicate> from_global_point_map(const std::string pred_string, std::map<std::string, std::unique_ptr<Point>> &global_point_map);

	std::string to_string();
};

class PredVec {
public:
	std::vector<Predicate*> preds;

	PredVec() {}
	PredVec(std::initializer_list<Predicate*> init_list) : preds(init_list) {}

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