#pragma once

#include <string>
#include <concepts>
#include <vector>
#include <map>
#include <array>

#include "Common/Generator.hh"
#include "DD/Predicate.hh"

/* Generic Node class. Nodes abstract geometric objects in the geometric graph. 

Invariant: In all cases, a root node should contain the most up-to-date record of the objects that its underlying object has a
relationship to. The other objects need not be stored as their root nodes at all times. */
class Node {

public:
    std::string name;

    Node* parent = nullptr;
    Node* root = nullptr;
    Predicate* parent_why = nullptr;

    Node(std::string name) : name(name), root(this) {}

    bool is_root();
};

namespace NodeUtils {

/* Returns the root of any `Node` object. 
This function has the secondary purpose of lazily updating the `root` pointer for every node it passes through to the correct root. */
template <std::derived_from<Node> Key>
Key* get_root(Key* n);

template<std::derived_from<Node> Key, size_t n>
std::array<Key*, n> get_root(std::array<Key*, n>&& t);

/* Takes in as input a map of the form `std::map<Object*, T>`. This may be `on_line`, `on_circle`, `points` etc. 
Returns the roots of all keys in the map.

Note: No deduplication occurs. */
template <std::derived_from<Node> Key, typename Map>
Generator<Key*> on_roots(Map& m);

/* Takes in as input a map of the form `std::map<Object*, T>`. This may be `on_line`, `on_circle`, `points` etc. 
Returns the roots of all keys in the map.

Note: Deduplication is handled in this version. */
template <std::derived_from<Node> Key, typename Map>
Generator<Key*> on_roots_dedup(Map& m);

/* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `points`. 
Returns all possible pairs of keys in the map. */
template <std::derived_from<Node> Key, typename Map>
Generator<std::pair<Key*, Key*>> on_pairs(Map& m);

/* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `points`. 
Returns all possible triples of keys in the map. */
template <std::derived_from<Node> Key, typename Map>
Generator<std::tuple<Key*, Key*, Key*>> on_triples(Map& m);

} // namespace NodeUtils