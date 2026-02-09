
#include <doctest.h>
#include <iostream>

#include "Geometry/GeometricGraph.hh"
#include "Common/Exceptions.hh"
#include "Geometry/Node.hh"

TEST_SUITE("GeometricGraph: Triangle congruence and similarity") {
    TEST_CASE("Triangle permutation") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a");
        Point* b = ggraph.__add_new_point("b");
        Point* c = ggraph.__add_new_point("c");

        Triangle* t = ggraph.get_or_add_triangle(a, b, c, base_pred);
        std::array<int, 3> perm1 = t->get_perm({c, a, b});
        REQUIRE(perm1 == std::array<int, 3>{2, 0, 1});
        t->permute(perm1);
        REQUIRE(t->vertices == std::array<Point*, 3>{c, a, b});

        std::array<int, 3> perm2 = {1, 2, 0};
        // TODO: Write compose_perm tests with hardcoded inputs
    }
    TEST_CASE("Setting congruence") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a");
        Point* b = ggraph.__add_new_point("b", {1, 0});
        Point* c = ggraph.__add_new_point("c", {1, 1});
        Point* d = ggraph.__add_new_point("d", {2, 1});
        Point* e = ggraph.__add_new_point("e", {2, 0});

        // Oppositely oriented congruent triangles: ABC ~ BCD
        ggraph.__make_contri(a, b, c, b, c, d, nullptr, dd, ar);
        REQUIRE((
            ggraph.check_cong(a, b, b, c) &&
            ggraph.check_cong(b, c, c, d) &&
            ggraph.check_cong(a, c, b, d)
        ));
        REQUIRE((
            ggraph.check_eqangle(a, b, b, c, d, c, c, b) &&
            ggraph.check_eqangle(b, c, c, a, b, d, d, c) &&
            ggraph.check_eqangle(c, a, a, b, c, b, b, d)
        ));

        // Similarly oriented congruent triangles: BDE ~ DBC <-> ABC ~ BCD ~ DEB
        ggraph.__make_contri(b, d, e, d, b, c, nullptr, dd, ar);
        REQUIRE(ggraph.check_cong(b, e, d, e));
        // BCD, DEB similarly oriented
        REQUIRE((
            ggraph.check_eqangle(d, b, b, e, b, d, d, c) &&
            ggraph.check_eqangle(b, c, c, d, d, e, e, b)
        ));
        // ABC, DEB oppositely oriented
        REQUIRE((
            ggraph.check_eqangle(b, d, d, e, b, a, a, c) &&
            ggraph.check_eqangle(d, b, b, e, b, c, c, a)
        ));

        Triangle* t1 = ggraph.get_or_add_triangle(a, b, c, base_pred);
        Triangle* t2 = ggraph.get_or_add_triangle(b, c, d, base_pred);
        Triangle* t3 = ggraph.get_or_add_triangle(b, d, e, base_pred);

        std::array<int, 3> perm1 = t1->get_perm({c, a, b});
        std::array<int, 3> perm2 = t2->get_perm({d, b, c});
        std::array<int, 3> perm3 = t3->get_perm({b, d, e});

        std::cout << "Perm1: " << perm1[0] << " " << perm1[1] << " " << perm1[2] << std::endl;
        std::cout << "Perm2: " << perm2[0] << " " << perm2[1] << " " << perm2[2] << std::endl;
        std::cout << "Perm3: " << perm3[0] << " " << perm3[1] << " " << perm3[2] << std::endl;
    }
    TEST_CASE("Setting similarity") {

    }
    TEST_CASE("Setting isosceles and equilateral mask") {

    }
    TEST_CASE("Right-angled isosceles and equilateral triangles") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {0, -2});
        Point* b = ggraph.__add_new_point("b", {-1, -1});
        Point* c = ggraph.__add_new_point("c", {0, -1});
        Point* d = ggraph.__add_new_point("d", {1, -1});
        Point* e = ggraph.__add_new_point("e", {-2, 0});
        Point* f = ggraph.__add_new_point("f", {0, 0});
        Point* g = ggraph.__add_new_point("g", {2, 0});
        Point* h = ggraph.__add_new_point("h", {-1, std::sqrt(3)});
        Point* i = ggraph.__add_new_point("i", {1, std::sqrt(3)});
    }
}