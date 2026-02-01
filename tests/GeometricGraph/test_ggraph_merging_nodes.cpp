
#include <doctest.h>
#include <iostream>

#include "Geometry/GeometricGraph.hh"
#include "Common/Exceptions.hh"

TEST_SUITE("GeometricGraph: Node merging") {
    TEST_CASE("Merging Object nodes") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
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
            ggraph.merge_points(a, b, base_pred, ar);
            ggraph.merge_points(c, d, base_pred, ar);
            ggraph.merge_points(a, c, base_pred, ar);
            ggraph.merge_points(e, f, base_pred, ar);
            ggraph.merge_points(g, h, base_pred, ar);
            ggraph.merge_points(f, h, base_pred, ar);
            ggraph.merge_points(d, h, base_pred, ar);

            bool all_pass = true;
            for (Point* p : {b, c, d, e, f, g, h}) {
                all_pass = all_pass && (NodeUtils::same_as(p, a)) && (NodeUtils::get_root(p) == a);
            }
            REQUIRE(all_pass);
        }
        SUBCASE("Line incidence from merging points") {
            ggraph.__set_point_numeric(b, {1, 0});
            ggraph.__set_point_numeric(d, {1, 0});
            ggraph.__set_point_numeric(f, {1, 0});
            ggraph.__set_point_numeric(h, {1, 0});

            Line* ab = ggraph.get_or_add_line(a, b, dd);
            d->set_this_on(ab, base_pred);
            REQUIRE(ggraph.get_or_add_line(a, d, dd) == ab);

            Line* cd = ggraph.get_or_add_line(c, d, dd);
            auto &m0 = cd->points; // {c, d}
            REQUIRE((m0.size() == 2 && m0.contains(c) && m0.contains(d)));

            ggraph.merge_points(a, c, base_pred, ar);
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

            ggraph.merge_points(b, f, base_pred, ar);
            ggraph.merge_points(d, h, base_pred, ar);
            ggraph.merge_points(e, g, base_pred, ar);

            // At this point, we have four clusters of points (a#, c), (b#, f), (d#, h), (e#, g)
            // and three lines (ab#, cd), (ef#), (gh#)

            REQUIRE((ef->is_root() && gh->is_root()));

            ggraph.merge_points(f, h, base_pred, ar);
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

            ggraph.merge_points(a, e, base_pred, ar);
            REQUIRE(NodeUtils::same_as(ab, ef));
            REQUIRE((ggraph.root_lines.size() == 1 && ggraph.root_lines.contains(NodeUtils::get_root(ab))));
            REQUIRE((ggraph.root_points.size() == 2 && ggraph.root_points.contains(a) && ggraph.root_points.contains(b)));
        }
        SUBCASE("Line incidence from merging lines") {
            ggraph.__set_point_numeric(a, {0, 0});
            ggraph.__set_point_numeric(b, {1, 0});
            ggraph.__set_point_numeric(c, {2, 0});
            ggraph.__set_point_numeric(d, {3, 0});
            ggraph.__set_point_numeric(e, {4, 0});
            ggraph.__set_point_numeric(f, {5, 0});
            ggraph.__set_point_numeric(g, {3, 0});  // d, g are numerically equal
            ggraph.__set_point_numeric(h, {0, 0});  // a, h are numerically equal

            Line* l1 = ggraph.get_or_add_line(a, b, dd);
            Line* l2 = ggraph.get_or_add_line(b, c, dd);
            Line* l3 = ggraph.get_or_add_line(c, d, dd);
            Line* l4 = ggraph.get_or_add_line(d, e, dd);
            Line* l5 = ggraph.get_or_add_line(e, f, dd);
            Line* l6 = ggraph.get_or_add_line(e, g, dd);
            Line* l7 = ggraph.get_or_add_line(g, h, dd);

            a->set_this_on(l3, base_pred);  // a, c, d
            b->set_this_on(l4, base_pred);  // b, d, e
            a->set_this_on(l5, base_pred);  // a, e, f
            c->set_this_on(l6, base_pred);  // c, e, g
            d->set_this_on(l7, base_pred);  // d, g, h
            /* On first round, [ab, bc] generates (acd, (a, c)) for L1
            On second round, [bc, acd] generates (bde, (b, d))
            and [ab, acd] also generates (bde, (b, d)) for L1
            On third round, [ab, bde] generates (aef, (a, e))
            and [bc, bde] generates (ceg, (c, e))
            and [acd, bde] generates both (aef, (a, e)) and (ceg, (c, e)) for L1 
            On fourth round, [acd, ceg] generates (dgh, (d, g))
            and [bde, ceg] generates (dgh, (d, h)) 
            both of which fail the numeric equality test */

            // Invoke coll a b c
            ggraph.merge_lines(l1, l2, base_pred, ar);

            // Now, all lines should have root equal to l1, except l7
            REQUIRE((
                NodeUtils::get_root(l2) == l1 &&
                NodeUtils::get_root(l3) == l1 &&
                NodeUtils::get_root(l4) == l1 &&
                NodeUtils::get_root(l5) == l1 &&
                NodeUtils::get_root(l6) == l1
            ));
            REQUIRE(NodeUtils::get_root(l7) == l7);
            for (Point* p : {a, b,c, e, f}) {
                REQUIRE((
                    p->on_root_line.size() == 1 && p->on_root_line.contains(l1) &&
                    l1->contains(p)
                ));
            }
            for (Point* p : {d, g}) {
                REQUIRE((
                    p->on_root_line.size() == 2 &&
                    p->on_root_line.contains(l1) &&
                    p->on_root_line.contains(l7) &&
                    l1->contains(p) &&
                    l7->contains(p)
                ));
            }

            // At this point, we have two lines:
            // l1 contains a, b, c, d, e, f, g
            // l7 contains d, g, h

            ggraph.merge_points(a, h, base_pred, ar);
            REQUIRE(NodeUtils::same_as(l1, l7));
            REQUIRE((ggraph.root_lines.size() == 1));
        }
        SUBCASE("Circle incidence from merging points") {
            ggraph.__set_point_numeric(c, {1, 0});
            ggraph.__set_point_numeric(d, {1, 0});
            ggraph.__set_point_numeric(e, {1, 1});
            ggraph.__set_point_numeric(f, {1, 1});
            ggraph.__set_point_numeric(g, {0, 1});
            ggraph.__set_point_numeric(h, {0, 1});

            Circle* ace = ggraph.get_or_add_circle(a, c, e, dd);
            Circle* bdf = ggraph.get_or_add_circle(b, d, f, dd);
            g->set_this_on(bdf, base_pred);
            REQUIRE(ggraph.get_or_add_circle(b, d, g, dd) == bdf);

            REQUIRE_THROWS_AS(ggraph.get_or_add_circle(b, c, d, dd), NumericsInternalError);
            Circle* bde = ggraph.get_or_add_circle(b, d, e, dd);
            c->set_this_on(bde, base_pred);

            ggraph.merge_points(a, b, base_pred, ar);
            ggraph.merge_points(c, d, base_pred, ar);

            // At this point we have six clusters of points (a#, b), (c#, d), (e#), (f#), (g#), (h#)
            // and two circles (r_bcde#, ?_ace), (bdfg)

            REQUIRE(NodeUtils::same_as(ace, bde));
            REQUIRE_FALSE(NodeUtils::same_as(bdf, bde));
            Circle* r_bde = NodeUtils::get_root(bde);
            auto &m0 = r_bde->points; // {a, c, e}
            REQUIRE((m0.size() == 3 && m0.contains(a) && m0.contains(c) && m0.contains(e)));
            auto &s0 = c->on_root_circle; // {r_bde, bdf}
            REQUIRE((s0.size() == 2 && s0.contains(r_bde) && s0.contains(bdf)));

            REQUIRE(ggraph.get_or_add_circle(a, f, g, dd) == bdf);
            Circle* aeh = ggraph.get_or_add_circle(a, e, h, dd);

            Circle* e_g = ggraph.get_or_add_circle(e, g, dd);
            Circle* f_h = ggraph.get_or_add_circle(f, h, dd);
            Circle* g_e = ggraph.get_or_add_circle(g, e, dd);
            Circle* h_f = ggraph.get_or_add_circle(h, f, dd);
            auto &s1 = h->center_of_root_circle; // {h_f}
            REQUIRE((s1.size() == 1 && s1.contains(h_f)));
            auto &s2 = h->on_root_circle; // {aeh, f_h}
            REQUIRE((s2.size() == 2 && s2.contains(aeh) && s2.contains(f_h)));

            ggraph.merge_points(g, h, base_pred, ar);

            // At this point we have five clusters of points (a#, b), (c#, d), (e#), (f#), (g#, h)
            // three circumcircles (r_bde#, ?_ace), (bdfg#), (aeh#)
            // and four more circles (e_g#), (f_h#), (g_e#), (h_f#)

            REQUIRE_FALSE(NodeUtils::same_as(bdf, aeh));
            REQUIRE((f->is_root() && bdf->is_root()));
            REQUIRE_FALSE((NodeUtils::same_as(e_g, f_h) || NodeUtils::same_as(g_e, h_f)));
            REQUIRE(f_h->points.contains(g));
            REQUIRE(h_f->get_center() == g);
            auto &m1 = NodeUtils::get_root(h)->on_root_circle; // {bdfg, aeh, e_g, f_h}
            REQUIRE((m1.size() == 4 && m1.contains(bdf) && m1.contains(aeh) && m1.contains(e_g) && m1.contains(f_h)));

            REQUIRE(ggraph.root_circles.size() == 7);

            ggraph.merge_circles(e_g, f_h, base_pred, ar); // merges the centers e and f
            REQUIRE(NodeUtils::same_as(e, f));

            // After this final merge, we now have four clusters of points (a#, b), (c#, d), (e#, f), (g#, h)
            // a single circumcircle (r_bde?, ace?, r_bdfg?, ?_aeh)
            // and two more circles (e_g#, f_h), (g_e#, h_f)

            Circle* r = NodeUtils::get_root(bdf);
            REQUIRE(f_h->get_center() == e);
            REQUIRE((
                NodeUtils::same_as(ace, r) &&
                NodeUtils::same_as(bde, r) &&
                NodeUtils::same_as(aeh, r) &&
                NodeUtils::same_as(e_g, f_h) &&
                NodeUtils::same_as(g_e, h_f)
            ));
            REQUIRE_FALSE((
                NodeUtils::same_as(e_g, g_e)
            ));
            auto &s3 = ggraph.root_circles; // {r, e_g, g_e}
            REQUIRE((s3.size() == 3 && s3.contains(r) && s3.contains(e_g) && s3.contains(g_e)));
            auto &m2 = r->points; // {a, c, e, g}
            REQUIRE((m2.size() == 4 && m2.contains(a) && m2.contains(c) && m2.contains(e) && m2.contains(g)));
            auto &m3 = g->on_root_circle; // {r, e_g}
            REQUIRE((m3.size() == 2 && m3.contains(r) && m3.contains(e_g)));
            auto &m4 = e_g->points; // {g}
            REQUIRE((m4.size() == 1 && m4.contains(g)));
        }
        SUBCASE("Segment incidence from merging points") {
            ggraph.__set_point_numeric(b, {0, 1});
            ggraph.__set_point_numeric(d, {0, 1});
            ggraph.__set_point_numeric(f, {0, 1});
            ggraph.__set_point_numeric(h, {0, 1});

            Segment* ab = ggraph.get_or_add_segment(a, b, dd);
            Segment* cd = ggraph.get_or_add_segment(c, d, dd);
            Segment* bc = ggraph.get_or_add_segment(b, c, dd);
            REQUIRE((bc->endpoints[0] == c && bc->endpoints[1] == b));
            
            ggraph.merge_points(a, c, base_pred, ar);
            REQUIRE(NodeUtils::same_as(ab, bc));
            REQUIRE_FALSE(NodeUtils::same_as(ab, cd));

            ggraph.merge_points(b, d, base_pred, ar);
            REQUIRE(NodeUtils::same_as(cd, ab));
            Segment* r_ab = NodeUtils::get_root(ab);
            REQUIRE(ggraph.get_or_add_segment(a, d, dd) == r_ab);

            auto &s0 = b->endpoint_of_root_segment; // {r_ab}
            REQUIRE((s0.size() == 1 && s0.contains(r_ab)));

            // At this point, we have six clusters of points (a#, c), (b#, d), (e#), (f#), (g#), (h#)
            // and one segment (r_ab#, ?_bc, ?_cd)
            Segment* ef = ggraph.get_or_add_segment(e, f, dd);
            Segment* gh = ggraph.get_or_add_segment(g, h, dd);
            Segment* eh = ggraph.get_or_add_segment(e, h, dd);
            Segment* gb = ggraph.get_or_add_segment(g, b, dd);
            Segment* cf = ggraph.get_or_add_segment(c, f, dd);

            ggraph.merge_points(e, g, base_pred, ar);

            REQUIRE(NodeUtils::same_as(eh, gh));
            Segment* r_eh = NodeUtils::get_root(eh);

            auto &s1 = e->endpoint_of_root_segment; // {ef, r_eh, gb}
            REQUIRE((s1.size() == 3 && s1.contains(ef) && s1.contains(r_eh) && s1.contains(gb)));

            ggraph.merge_points(b, f, base_pred, ar);
            REQUIRE((
                NodeUtils::same_as(r_ab, cf) &&
                NodeUtils::same_as(ef, gb)
            ));
            r_ab = NodeUtils::get_root(r_ab);
            Segment* r_ef = NodeUtils::get_root(gb);
            auto &s2 = ggraph.root_segments; // {r_ab, r_eh, r_ef}
            REQUIRE((s2.size() == 3 && s2.contains(r_ab) && s2.contains(r_ef) && s2.contains(r_eh)));

            // At this point, we have four clusters of points (a#, c), (b#, d, f), (e#, g), (h#)
            // and three segments (r_ab#, ?_bc, ?_cd, ?_cf), (r_ef#, ?_gb), (r_eh#, ?_gh)

            ggraph.merge_points(h, f, base_pred, ar);
            REQUIRE(NodeUtils::same_as(r_ef, r_eh));
            REQUIRE_FALSE(NodeUtils::same_as(r_ab, r_ef));
            auto &s3 = h->endpoint_of_root_segment; // {r_ab, r_ef}
            REQUIRE((s3.size() == 2 && s3.contains(r_ab) && s3.contains(NodeUtils::get_root(r_ef))));
        }
    }
}