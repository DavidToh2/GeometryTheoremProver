
#include <set>
#include <iterator>

#include "Node.hh"

bool Node::is_root() {
    return (root == this);
}

namespace NodeUtils {

template <std::derived_from<Node> Key>
Key* get_root(Key* n) {
    if (n->root != n) {
        n->root = get_root(n->root);
    }
    return n->root;
}

template<std::derived_from<Node> Key, size_t n>
std::array<Key*, n> get_root(std::array<Key*, n>&& t) {
    std::array<Key*, n> res;
    for (size_t i=0; i<n; i++) {
        res[i] = get_root(t[i]);
    }
    return res;
}

template <std::derived_from<Node> Key, typename Map>
Generator<Key*> on_roots(Map& m) {
    for (const auto& [key, _] : m) {
        Node* r = get_root(key);
        if (r != key) {
            co_yield static_cast<Key*>(r);
        }
    }
    co_return;
}

template <std::derived_from<Node> Key, typename Map>
Generator<Key*> on_roots_dedup(Map& m) {
    std::set<Node*> yielded;
    for (const auto& [key, _] : m) {
        Node* r = get_root(key);
        if ((r != key) && !(yielded.contains(r))) {
            yielded.insert(r);
            co_yield static_cast<Key*>(r);
        }
    }
    co_return;
}

template <std::derived_from<Node> Key, typename Map>
Generator<std::pair<Key*, Key*>> on_pairs(Map& m) {
    for (std::iterator it = m.begin(); it != m.end(); ++it) {
        for (std::iterator jt = std::next(it); jt != m.end(); ++jt) {
            co_yield {static_cast<Key*>(it->first), static_cast<Key*>(jt->first)};
        }
    }
    co_return;
}

template <std::derived_from<Node> Key, typename Map>
Generator<std::tuple<Key*, Key*, Key*>> on_triples(Map& m) {
    for (std::iterator it = m.begin(); it != m.end(); ++it) {
        for (std::iterator jt = std::next(it); jt != m.end(); ++jt) {
            for (std::iterator kt = std::next(jt); kt != m.end(); ++kt) {
                co_yield {static_cast<Key*>(it->first), static_cast<Key*>(jt->first), static_cast<Key*>(kt->first)};
            }
        }
    }
    co_return;
}

} // namespace NodeUtils