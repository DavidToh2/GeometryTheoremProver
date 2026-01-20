#pragma once 

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Common/Frac.hh"
#include "Common/Generator.hh"
#include "Common/Constants.hh"
#include "Common/Utils.hh"
#include "Common/Arg.hh"

class GeometricGraph;

class PredicateTemplate {
	std::string id;

public:
	pred_t name;
	std::vector<Arg*> args;
	
	PredicateTemplate(const pred_t name) : name(name) {}
	PredicateTemplate(const std::string name) : name(Utils::to_pred_t(name)) {}

	PredicateTemplate(const pred_t name, std::vector<Arg*> &args) 
	: name(name), args(std::move(args)) {}
	PredicateTemplate(const std::string name, std::vector<Arg*> &args) 
	: name(Utils::to_pred_t(name)), args(std::move(args)) {}

	/* This constructor is used to parse predicate strings from textual input. 
	Note: Assumes the input is already StrUtils::strip()-ed */ 
	PredicateTemplate(const std::string pred_string, std::map<std::string, Arg*> &argmap);

	// This constructor literally only exists to convert the conclusion from a Predicate to a PredicateTemplate
	PredicateTemplate(Predicate* pred, std::vector<std::unique_ptr<Arg>> &arglist);

	/* See the `Arg` class documentation for more details about the return values. In short:
	- Unsuccessful set, returns 0, if the argument was already set to a different `Node*` 
	- Successful set, returns 1: if the argument was previously empty
	- Unchanged set, returns 2: if the argument was already set to the same `Node*`*/
	char set_arg(int i, Node* node);
	char set_arg(int i, Frac f);
	char set_arg(int i, char c);
	bool arg_empty(int i) const noexcept;
	void clear_arg(int i) noexcept;

	/* Note: Returns 0 or 1 depending on whether there were any unsuccessful sets */
	char set_args(std::vector<Node*> nodes);
	/* Note: Returns 0 or 1 depending on whether there were any unsuccessful sets */
	char set_args(std::vector<Node*> nodes, Frac f);
	bool args_filled() const;
	void clear_args();

	Point* get_arg_point(int i);

	std::unique_ptr<Predicate> instantiate();

	std::string to_string() const;
	std::string to_hash_with_args() const;

	bool __validate_neq(GeometricGraph &ggraph);
	bool __validate_ncoll(GeometricGraph &ggraph);
	bool validate_degeneracy_args(GeometricGraph &ggraph);
};

class PredVec {
public:
	std::vector<Predicate*> preds;

	PredVec() {}
	PredVec(std::initializer_list<Predicate*> init_list) : preds(init_list) {}
	PredVec(std::vector<Predicate*> &&vec) : preds(std::move(vec)) {}

	void operator+=(Predicate* pred);
	void operator+=(const PredVec& other);

	explicit operator std::vector<Predicate*>() const { return preds; }

	std::string to_string() const;
};

class Predicate {
public:
	std::string hash;

	pred_t name;
	std::vector<Node*> args;
	Frac frac_arg;
	PredVec why; // keep

	Predicate() : name(pred_t::BASE), hash(Utils::to_pred_str(pred_t::BASE)) {}
	Predicate(const pred_t name, std::vector<Node*> &&nodes);
	Predicate(const pred_t name, std::vector<Node*> &&nodes, Frac f);
	Predicate(const pred_t name, std::vector<Node*> &&nodes, PredVec &&why);
	Predicate(const pred_t name, std::vector<Node*> &&nodes, Frac f, PredVec &&why);
	Predicate(const pred_t name, std::vector<Node*> &&nodes, std::vector<Predicate*> &&why);
	Predicate(const pred_t name, std::vector<Node*> &&nodes, Frac f, std::vector<Predicate*> &&why);
	Predicate(PredicateTemplate &pred_template);

	static std::unique_ptr<Predicate> 
	from_global_point_map(const std::string pred_string, std::map<std::string, std::unique_ptr<Point>> &global_point_map);

	std::string to_string() const;
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