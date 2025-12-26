
#include <doctest.h>
#include <iostream>

#include "AR/Table.hh"

TEST_SUITE("Table") {
    TEST_CASE("update_equal_groups") {
        std::set<std::set<int>> g1 = {
            {1, 2, 3, 4, 5},
            {7, 8, 9},
            {10, 11},
            {14, 15, 16},
            {20, 21},
            {22, 23, 24},
            {27, 28, 29, 30, 31}
        };

        std::set<std::set<int>> g2 = {
            {2, 4, 6, 8},
            {11, 13, 14, 16},
            {3, 5, 10,12, 15},
            {25, 26},
            {21, 23, 28, 29, 31}
        };

        std::set<std::set<int>> g1_copy = g1;
        std::set<std::set<int>> g2_copy = g2;
        std::vector<std::pair<int, int>> links;
        for (const auto& g : g2) {
            Table::update_equal_groups(g1, g, links);
        }

        std::set<std::set<int>> expected_groups = {
            {1,2,3,4,5,6,7, 8, 9, 10, 11, 12, 13, 14, 15, 16},
            {25, 26},
            {20,21,22,23,24,27,28,29,30,31}
        };

        // for (const auto l : links) {
        //     std::cout << l.first << "-" << l.second << ",";
        // }
        // Because sets 0, 1, 2, 3 are merged, sets 4, 5, 6 are merged, and new elements
        // 6, 12, 13, 25, 26 are added with (25, 26) being by themselves: 
        // 5 merges + 5 new elements - 1 new set = 9 links expected.
        CHECK(links.size() == 9);

        CHECK(g1.size() == expected_groups.size());
        for (const auto& g : g1) {
            CHECK(expected_groups.contains(g));
        }
        CHECK(g2 == g2_copy);
    }
}