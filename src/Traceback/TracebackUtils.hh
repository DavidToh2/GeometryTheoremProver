
#pragma once

#include <deque>

#include "Common/Exceptions.hh"
#include "Geometry/Node.hh"

namespace TracebackUtils {

    template<std::derived_from<Node> T>
    std::pair<T*, int> lowest_common_ancestor(T* n1, T* n2, T* n3) {
        std::array<std::vector<T*>, 3> parents;
        std::array<T*, 3> ns{n1, n2, n3};
        for (int i=0; i<3; i++) {
            parents[i].emplace_back(ns[i]);
            while (!ns[i]->is_root()) {
                ns[i] = NodeUtils::get_parent(ns[i]);
                parents[i].emplace_back(ns[i]);
            }
        }
        std::array<int, 3> sizes{
            static_cast<int>(parents[0].size())-1, 
            static_cast<int>(parents[1].size())-1, 
            static_cast<int>(parents[2].size())-1
        };
        while ((parents[0][sizes[0]] == parents[1][sizes[1]]) && (parents[1][sizes[1]] == parents[2][sizes[2]])) {
            sizes[0]--; sizes[1]--; sizes[2]--;
        }
        sizes[0]++; sizes[1]++; sizes[2]++;
        T* lcp = parents[0][sizes[0]];
        return std::pair<T*, int>(lcp, sizes[0] + sizes[1] + sizes[2]);
    }

    template<std::derived_from<Node> T>
    PredSet why_ancestor(T* child, T* ancestor) {
        PredSet res;
        while (child != ancestor) {
            if (child->is_root()) {
                throw TracebackInternalError("TracebackUtils::why_ancestor(): child is not a descendant of ancestor");
            }
            std::deque<Predicate*> preds{child->parent_why};
            while (!preds.empty()) {
                Predicate* pred = preds.front();
                // Decompose all EQ predicates
                if (pred->name == pred_t::EQ) {
                    for (Predicate* p_why : pred->why.preds) {
                        if (p_why) preds.emplace_back(p_why);
                    }
                } else {
                    res += pred;
                }
                preds.pop_front();
            }
            child = NodeUtils::get_parent(child);
        }
        return res;
    }


} // namespace TracebackUtils