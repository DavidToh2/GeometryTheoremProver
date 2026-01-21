
#include <doctest.h>
#include <iostream>

#include "Geometry/GeometricGraph.hh"

TEST_SUITE("GeometricGraph: Node merging") {
    TEST_CASE("Merging Object nodes") {
        GeometricGraph ggraph;
        DDEngine dd;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a");
        Point* b = ggraph.__add_new_point("b");
        Point* c = ggraph.__add_new_point("c");
        Point* d = ggraph.__add_new_point("d");
        Point* e = ggraph.__add_new_point("e");
        Point* f = ggraph.__add_new_point("f");
        Point* g = ggraph.__add_new_point("g");
        Point* h = ggraph.__add_new_point("h");

        SUBCASE("Basic parent/root hierarchy behaviour") {
            ggraph.merge_points(a, b, base_pred);
            ggraph.merge_points(c, d, base_pred);
            ggraph.merge_points(a, c, base_pred);
            ggraph.merge_points(e, f, base_pred);
            ggraph.merge_points(g, h, base_pred);
            ggraph.merge_points(f, h, base_pred);
            ggraph.merge_points(d, h, base_pred);

            bool all_pass = true;
            for (Point* p : {b, c, d, e, f, g, h}) {
                all_pass = all_pass && (NodeUtils::same_as(p, a)) && (NodeUtils::get_root(p) == a);
            }
            CHECK(all_pass);
        }
        SUBCASE("Line incidence") {
            Line* ab = ggraph.get_or_add_line(a, b, dd);
            Line* ac = ggraph.get_or_add_line(a, c, dd);
            Line* bc = ggraph.get_or_add_line(b, c, dd);
            ggraph.merge_lines(ab, ac, base_pred);
            

            Line* ef = ggraph.get_or_add_line(e, f, dd);
        }
        SUBCASE("Circle incidence") {

        }
        SUBCASE("Segment incidence") {

        }
    }
}