
#pragma once

#include <deque>

#include "Common/Exceptions.hh"
#include "Geometry/Node.hh"

namespace TracebackUtils {

    template <std::derived_from<Node> T, std::same_as<T*>... Args>
    std::pair<T*, int> lowest_common_ancestor(T* first, Args... rest) {
        constexpr std::size_t N = 1 + sizeof...(Args);
        
        std::array<std::vector<T*>, N> parents;
        std::array<T*, N> ns{first, rest...};
        
        for (std::size_t i = 0; i < N; i++) {
            parents[i].emplace_back(ns[i]);
            while (!ns[i]->is_root()) {
                ns[i] = NodeUtils::get_parent(ns[i]);
                parents[i].emplace_back(ns[i]);
            }
        }
        
        std::array<int, N> sizes;
        for (std::size_t i = 0; i < N; i++) {
            sizes[i] = static_cast<int>(parents[i].size()) - 1;
        }
        
        while (true) {
            bool in_bounds = true;
            for (std::size_t i = 0; i < N; i++) {
                if (sizes[i] < 0) { 
                    in_bounds = false; 
                    break; 
                }
            }
            if (!in_bounds) break;

            T* current_ancestor = parents[0][sizes[0]];
            bool all_match = true;
            for (std::size_t i = 1; i < N; i++) {
                if (parents[i][sizes[i]] != current_ancestor) {
                    all_match = false;
                    break;
                }
            }
            
            if (!all_match) break;
            
            for (std::size_t i = 0; i < N; i++) {
                sizes[i]--;
            }
        }
        
        int total_distance = 0;
        for (std::size_t i = 0; i < N; i++) {
            sizes[i]++;
            total_distance += sizes[i];
        }
        
        T* lcp = parents[0][sizes[0]];
        return std::pair<T*, int>(lcp, total_distance);
    }

    template<std::derived_from<Node> T>
    PredSet why_ancestor(T* child, T* ancestor) {
        PredSet res;
        while (child != ancestor) {
            if (child->is_root()) {
                return {};
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