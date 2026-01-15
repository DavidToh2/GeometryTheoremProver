
#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <variant>

#include "Frac.hh"

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

	/* Given a string of Argument names (e.g. `p q r s t`), add new `std::unique_ptr<Arg>` and `Arg*` entries
	to the provided maps `arg_unique_ptr_vec` and `argmap` respectively, one corresponding to each Arg name, provided
	they are not already present 
	Note: If an Arg name is already present in `argmap`, then nothing happens */
	static void populate_args_and_argmap(
		const std::string arg_str, 
		std::vector<std::unique_ptr<Arg>> &arg_unique_ptr_vec, 
		std::map<std::string, Arg*> &argmap);

	void operator=(Node* node);
	void operator=(Frac f);
	void operator=(char c);

	std::string to_string();
};