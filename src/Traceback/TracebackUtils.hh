
#pragma once

#include "Common/Exceptions.hh"
#include "Geometry/Node.hh"

namespace TracebackUtils {

    template<std::derived_from<Node> T>
    std::pair<T*, int> lowest_common_ancestor(T* n1, T* n2, T* n3) {
        std::array<std::vector<T*>, 3> parents;
        std::array<T*, 3> ns{n1, n2, n3};
        for (int i=0; i<3; i++) {
            parents[i].emplace_back(ns[i]);
            do {
                ns[i] = static_cast<T*>(ns[i]->parent);
                parents[i].emplace_back(ns[i]);
            } while (!ns[i]->is_root());
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
            res += child->parent_why;
            child = NodeUtils::get_parent(child);
        }
        return res;
    }


} // namespace TracebackUtils