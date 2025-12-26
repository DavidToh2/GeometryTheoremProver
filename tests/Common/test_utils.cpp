
#include "doctest.h"

#include "Common/Utils.hh"

TEST_SUITE("Utils") {
    TEST_CASE("Set intersection") {
        std::set<int> set1 = {1, 2, 3, 4};
        std::set<int> set2 = {3, 4, 5, 6};
        std::set<int> expected_intersection = {3, 4};
        std::set<int> intersection = Utils::intersect_sets(set1, set2);
        CHECK(intersection == expected_intersection);
    }

    TEST_CASE("Set union") {
        std::set<int> set1 = {1, 2, 3};
        std::set<int> set2 = {3, 4, 5};
        std::set<int> expected_union = {1, 2, 3, 4, 5};
        std::set<int> union_set = Utils::unify_sets(set1, set2);
        CHECK(union_set == expected_union);
    }

    TEST_CASE("Set union in-place") {
        std::set<int> set1 = {1, 2, 3};
        std::set<int> set2 = {3, 4, 5};
        std::set<int> set2_copy = set2;
        std::set<int> expected_union = {1, 2, 3, 4, 5};
        Utils::__unify_sets(set1, set2);
        CHECK(set1 == expected_union);
        CHECK(set2 == set2_copy);
    }
}