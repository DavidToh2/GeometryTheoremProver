
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

        SUBCASE("Node parent/root hierarchy") {
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
            REQUIRE(all_pass);
        }
        SUBCASE("Line") {
            Line* ab = ggraph.get_or_add_line(a, b, dd);
            d->set_this_on(ab, base_pred);
            REQUIRE(ggraph.get_or_add_line(a, d, dd) == ab);

            Line* cd = ggraph.get_or_add_line(c, d, dd);
            auto &m0 = cd->points; // {c, d}
            REQUIRE((m0.size() == 2 && m0.contains(c) && m0.contains(d)));

            ggraph.merge_points(a, c, base_pred);
            Line* r_ab = NodeUtils::get_root(ab);
            REQUIRE(NodeUtils::same_as(ab, cd));

            auto &m1 = NodeUtils::get_root(cd)->points; // {a, b, d}
            REQUIRE((m1.size() == 3 && m1.contains(a) && m1.contains(b) && m1.contains(d)));
            for (Point* p : {a, b, d}) {
                REQUIRE((p->on_root_line.size() == 1 && p->on_root_line.contains(r_ab)));
            }
            auto &s1 = ggraph.root_lines;
            REQUIRE((s1.size() == 1 && s1.contains(r_ab)));

            // At this point, (a#, c) are merged. Here # means the root point

            Line* ef = ggraph.get_or_add_line(e, f, dd);
            Line* gh = ggraph.get_or_add_line(g, h, dd);

            ggraph.merge_points(b, f, base_pred);
            ggraph.merge_points(d, h, base_pred);
            ggraph.merge_points(e, g, base_pred);

            // At this point, we have four clusters of points (a#, c), (b#, f), (d#, h), (e#, g)
            // and three lines (ab#, cd), (ef#), (gh#)

            REQUIRE((ef->is_root() && gh->is_root()));

            ggraph.merge_points(f, h, base_pred);
            Line* r_ef = NodeUtils::get_root(ef);
            REQUIRE(NodeUtils::same_as(ef, gh));

            auto &s2 = r_ef->points; // {e, b}
            REQUIRE((s2.size() == 2 && s2.contains(e) && s2.contains(b)));
            auto &m2 = b->on_root_line; // {r_ab, r_ef}
            REQUIRE((m2.size() == 2 && m2.contains(r_ab) && m2.contains(r_ef)));
            auto &m3 = e->on_root_line; // {r_ef}
            REQUIRE((m3.size() == 1 && m3.contains(r_ef)));
            auto &s3 = ggraph.root_points;
            REQUIRE((s3.size() == 3 && s3.contains(a) && s3.contains(b) && s3.contains(e)));

            // At this point, we have three clusters of points (a#, c), (b#, f, d, h), (e#, g)
            // and two lines (r_ab#, ?_cd), (r_ef#, ?_gh)

            REQUIRE(ggraph.get_or_add_line(d, g, dd) == r_ef);

            ggraph.merge_points(a, e, base_pred);
            REQUIRE(NodeUtils::same_as(ab, ef));
            REQUIRE((ggraph.root_lines.size() == 1 && ggraph.root_lines.contains(NodeUtils::get_root(ab))));
            REQUIRE((ggraph.root_points.size() == 2 && ggraph.root_points.contains(a) && ggraph.root_points.contains(b)));
        }
        SUBCASE("Circle incidence") {

        }
        SUBCASE("Segment incidence") {

        }
    }
}