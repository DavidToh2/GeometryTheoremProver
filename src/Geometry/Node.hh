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
    Node* root = this;
    Predicate* parent_why = nullptr;

    Node(std::string name) : name(name), root(this) {}

    constexpr bool is_root() { return (root == this); }
    constexpr std::string to_string() { return name; }
};

namespace NodeUtils {

    /* Returns the root of any `Node` object. 
    This function has the secondary purpose of lazily updating the `root` pointer for every node it passes through to the correct root. */
    template <std::derived_from<Node> Key>
    Key* get_root(Key* n) {
        if (!n->is_root()) {
            n->root = get_root(n->root);
        }
        return static_cast<Key*>(n->root);
    }

    /* Returns the roots of all elements in an array of `Node` objects. */
    template<std::derived_from<Node> Key, int n>
    std::array<Key*, n> get_roots(std::array<Key*, n>&& t) {
        std::array<Key*, n> res;
        for (size_t i=0; i<n; i++) {
            res[i] = get_root(t[i]);
        }
        return res;
    }

    /* Checks if two objects have the same root. */
    template <std::derived_from<Node> Key>
    static bool same_as(Key* a, Key* b) {
        return (get_root(a) == get_root(b));
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. Returns all keys in the map. 
    Note: In ordinary conditions we won't need to use this, as we can simply use a regular iterator of the form `for (auto [key, _] : map) { ... }`*/
    template <std::derived_from<Node> Key, typename Map>
    Generator<Key*> all(Map& m) {
        for (const auto& [key, _] : m) {
            co_yield static_cast<Key*>(key);
        }
        co_return;
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `on_line`, `on_circle`, `points` etc. 
    Returns the roots of all keys in the map.

    Note: No deduplication occurs. */
    template <std::derived_from<Node> Key, typename Map>
    Generator<Key*> all_roots(Map& m) {
        for (const auto& [key, _] : m) {
            Node* r = get_root(key);
            co_yield static_cast<Key*>(r);
        }
        co_return;
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `on_line`, `on_circle`, `points` etc. 
    Returns all distinct roots of all keys in the map. */
    template <std::derived_from<Node> Key, typename Map>
    Generator<Key*> all_roots_dedup(Map& m) {
        std::set<Node*> yielded;
        for (const auto& [key, _] : m) {
            Node* r = get_root(key);
            if (!(yielded.contains(r))) {
                yielded.insert(r);
                co_yield static_cast<Key*>(r);
            }
        }
        co_return;
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `points`. 
    Returns all pairs of keys in the map. */
    template <std::derived_from<Node> Key, typename Map>
    Generator<std::pair<Key*, Key*>> all_pairs(Map& m) {
        for (auto it = m.begin(); it != m.end(); ++it) {
            for (auto jt = std::next(it); jt != m.end(); ++jt) {
                co_yield {static_cast<Key*>(it->first), static_cast<Key*>(jt->first)};
            }
        }
        co_return;
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `points`. 
    Returns all ordered pairs of keys in the map. */
    template <std::derived_from<Node> Key, typename Map>
    Generator<std::pair<Key*, Key*>> all_pairs_ordered(Map& m) {
        for (auto it = m.begin(); it != m.end(); ++it) {
            for (auto jt = std::next(it); jt != m.end(); ++jt) {
                Key* k1 = static_cast<Key*>(it->first);
                Key* k2 = static_cast<Key*>(jt->first);
                co_yield {k1, k2};
                co_yield {k2, k1};
            }
        }
        co_return;
    }

    /* Take in as input a set of pointers to `Value`s. This will usually be a `root_objs` set.
    Returns all pairs of values in the set. */
    template <std::derived_from<Node> Value>
    Generator<std::pair<Value*, Value*>> all_pairs(std::set<Value*>& s) {
        for (auto it = s.begin(); it != s.end(); ++it) {
            for (auto jt = std::next(it); jt != s.end(); ++jt) {
                co_yield {(*it), (*jt)};
            }
        }
        co_return;
    }

    /* Take in as input a set of pointers to `Value`s. This will usually be a `root_objs` set.
    Returns all ordered pairs of values in the set. */
    template <std::derived_from<Node> Value>
    Generator<std::pair<Value*, Value*>> all_pairs_ordered(std::set<Value*>& s) {
        for (auto it = s.begin(); it != s.end(); ++it) {
            for (auto jt = std::next(it); jt != s.end(); ++jt) {
                co_yield {(*it), (*jt)};
                co_yield {(*jt), (*it)};
            }
        }
        co_return;
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `objs`. 
    Returns all distinct pairs of roots of keys in the map. */
    template <std::derived_from<Node> Key, typename Map>
    Generator<std::pair<Key*, Key*>> all_root_pairs_dedup(Map& m) {
        std::vector<Node*> yielded_1;
        auto roots_1 = all_roots_dedup<Key>(m);
        while (roots_1) {
            yielded_1.emplace_back(roots_1());
            Key* r1 = yielded_1.back();
            for (auto it = yielded_1.begin(); it != yielded_1.end() - 1; ++it) {
                co_yield {(*it), r1};
            }
        }
        co_return;
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `points`. 
    Returns all triples of keys in the map. */
    template <std::derived_from<Node> Key, typename Map>
    Generator<std::tuple<Key*, Key*, Key*>> all_triples(Map& m) {
        for (auto it = m.begin(); it != m.end(); ++it) {
            for (auto jt = std::next(it); jt != m.end(); ++jt) {
                for (auto kt = std::next(jt); kt != m.end(); ++kt) {
                    co_yield {static_cast<Key*>(it->first), 
                        static_cast<Key*>(jt->first), 
                        static_cast<Key*>(kt->first)};
                }
            }
        }
        co_return;
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `points`. 
    Returns all ordered triples of keys in the map. */
    template <std::derived_from<Node> Key, typename Map>
    Generator<std::tuple<Key*, Key*, Key*>> all_triples_ordered(Map& m) {
        auto unordered_gen = all_triples<Key>(m);
        while (unordered_gen) {
            auto [k1, k2, k3] = unordered_gen();
            co_yield {k1, k2, k3};
            co_yield {k1, k3, k2};
            co_yield {k2, k1, k3};
            co_yield {k2, k3, k1};
            co_yield {k3, k1, k2};
            co_yield {k3, k2, k1};
        }
        co_return;
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `points`. 
    Returns all quadruples of keys in the map. */
    template <std::derived_from<Node> Key, typename Map>
    Generator<std::tuple<Key*, Key*, Key*, Key*>> all_quads(Map& m) {
        for (auto it = m.begin(); it != m.end(); ++it) {
            for (auto jt = std::next(it); jt != m.end(); ++jt) {
                for (auto kt = std::next(jt); kt != m.end(); ++kt) {
                    for (auto lt = std::next(kt); lt != m.end(); ++lt) {
                        co_yield {static_cast<Key*>(it->first), 
                            static_cast<Key*>(jt->first), 
                            static_cast<Key*>(kt->first), 
                            static_cast<Key*>(lt->first)};
                    }
                }
            }
        }
        co_return;
    }

    /* Takes in as input a map of the form `std::map<Object*, T>`. This will usually be `points`. 
    Returns all quadruples of keys in the map. */
    template <std::derived_from<Node> Key, typename Map>
    Generator<std::tuple<Key*, Key*, Key*, Key*>> all_quads_ordered(Map& m) {
        
        auto unordered_gen = all_quads<Key>(m);
        while (unordered_gen) {
            auto [k1, k2, k3, k4] = unordered_gen();
            co_yield {k1, k2, k3, k4};
            co_yield {k1, k2, k4, k3};
            co_yield {k1, k3, k2, k4};
            co_yield {k1, k3, k4, k2};
            co_yield {k1, k4, k2, k3};
            co_yield {k1, k4, k3, k2};

            co_yield {k2, k1, k3, k4};
            co_yield {k2, k1, k4, k3};
            co_yield {k2, k3, k1, k4};
            co_yield {k2, k3, k4, k1};
            co_yield {k2, k4, k1, k3};
            co_yield {k2, k4, k3, k1};

            co_yield {k3, k1, k2, k4};
            co_yield {k3, k1, k4, k2};
            co_yield {k3, k2, k1, k4};
            co_yield {k3, k2, k4, k1};
            co_yield {k3, k4, k1, k2};
            co_yield {k3, k4, k2, k1};

            co_yield {k4, k1, k2, k3};
            co_yield {k4, k1, k3, k2};
            co_yield {k4, k2, k1, k3};
            co_yield {k4, k2, k3, k1};
            co_yield {k4, k3, k1, k2};
            co_yield {k4, k3, k2, k1};
        }
        co_return;
    }

} // namespace NodeUtils