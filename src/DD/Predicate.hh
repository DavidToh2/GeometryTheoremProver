#pragma once 

#include <string>
#include <vector>
#include <set>
#include <variant>
#include <memory>
#include <Numerics/Numerics.hh>
#include <Geometry/Node.hh>
#include <Geometry/Object.hh>

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
};

class Predicate {
	std::string id;

public:
	std::string name;
	std::vector<Arg*> args;

	std::set<Predicate*> why;
	
	Predicate(const std::string name) : name(name) {}
	Predicate(const std::string name, std::vector<Arg*> &args) : name(name), args(std::move(args)) {};
	Predicate(const std::string pred_string, std::map<std::string, Arg*> &argmap);

	std::string to_string();

	void set_arg(int i, Object* obj) noexcept;
	void set_arg(int i, Frac f) noexcept;
	void set_arg(int i, char c) noexcept;
	void clear_arg(int i) noexcept;

	void set_args(std::vector<Object*> objs);
	void set_args(std::vector<Object*> objs, Frac f);
	void clear_args();
};

class Clause {

public:
	std::string name;
	std::vector<Arg*> args;
	std::vector<std::unique_ptr<Predicate>> predicates;

	Clause() {}
	Clause(std::string clause_string, std::map<std::string, Arg*> &argmap);

	void empty_args();

	// TODO: Write a generator that, given a proof state graph, produces all valid unifications of the clause


	std::string to_string();
};