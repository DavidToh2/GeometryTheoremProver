
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_coll()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {0, 0});
        Point* b = ggraph.__add_new_point("b", {1, 0});
        Point* c = ggraph.__add_new_point("c", {2, 0});
        Point* d = ggraph.__add_new_point("d", {0, 0});
        Point* e = ggraph.__add_new_point("e", {1, 0});
        Point* f = ggraph.__add_new_point("f", {2, 0});
        Point* g = ggraph.__add_new_point("g", {1, 0});
        Point* h = ggraph.__add_new_point("h", {2, 0});
        Point* i = ggraph.__add_new_point("i", {2, 0});

        Line* l1 = ggraph.__add_new_line(a, b, base_pred);
        Line* l2 = ggraph.__add_new_line(b, c, base_pred);
        Line* l3 = ggraph.__add_new_line(d, b, base_pred);
        Line* l4 = ggraph.__add_new_line(d, e, base_pred);
        Line* l5 = ggraph.__add_new_line(e, f, base_pred);
        Line* l6 = ggraph.__add_new_line(b, h, base_pred);
        Line* l7 = ggraph.__add_new_line(e, i, base_pred);
        Line* l8 = ggraph.__add_new_line(d, g, base_pred);
        Line* l9 = ggraph.__add_new_line(g, c, base_pred);
        Line* l10 = ggraph.__add_new_line(g, i, base_pred);

        std::vector<Predicate*> preds;

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{c, b, a})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{h, b, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{f, e, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{i, g, d})
        ));

        /* coll C B A, coll H B D, coll F E D, coll I G D */
        ggraph.synthesise_preds(dd, ar);

        REQUIRE((
            NodeUtils::get_root(l2) == l1 &&
            NodeUtils::get_parent(l2) == l1 &&
            NodeUtils::get_parent(l6) == l3 &&
            NodeUtils::get_parent(l5) == l4 &&
            NodeUtils::get_parent(l10) == l8
        ));

        PredSet why_on_c_l1 = tr.why_on(c, l1);
        REQUIRE((
            why_on_c_l1.size() == 2 &&
            why_on_c_l1.contains(base_pred) &&
            why_on_c_l1.contains(preds[0])  // coll A B C
        ));

        /* I = C */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{i, c}
            )
        ));
        ggraph.merge_points(i, c, preds.back(), dd, ar);

        REQUIRE((
            NodeUtils::get_parent(l9) == l8 &&
            NodeUtils::get_root(l9) == l8 &&
            NodeUtils::get_parent(c) == i &&
            NodeUtils::get_root(c) == i
        ));

        PredSet why_eq_l8_l9 = TracebackUtils::why_ancestor(l9, l8);
        REQUIRE((
            why_eq_l8_l9.size() == 3 &&
            why_eq_l8_l9.contains(base_pred) &&
            why_eq_l8_l9.contains(preds[3]) &&  // coll I G D
            why_eq_l8_l9.contains(preds[4])     // base I C
        ));


        /* D = A */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{d, a}
            )
        ));
        ggraph.merge_points(d, a, preds.back(), dd, ar);

        // The lines l3 = DBH and l1 = ABI are merged first, then the lines l8 = DGI and l1 = ABI

        REQUIRE((
            NodeUtils::get_parent(l10) == l8 && NodeUtils::get_parent(l9) == l8 && NodeUtils::get_parent(l3) == l8 &&
            NodeUtils::get_parent(l6) == l3 && NodeUtils::get_parent(l1) == l3 &&
            NodeUtils::get_parent(l2) == l1 && 
            NodeUtils::get_root(l2) == l8 &&
            l8->children.size() == 3 && l3->children.size() == 2 && l1->children.size() == 1
        ));
        REQUIRE((
            NodeUtils::get_root(a) == d
        ));

        PredSet why_eq_l3_l1 = TracebackUtils::why_ancestor(l1, l3);
        REQUIRE((
            why_eq_l3_l1.size() == 2 &&
            why_eq_l3_l1.contains(base_pred) &&
            why_eq_l3_l1.contains(preds[5])  // base D A
        ));

        PredSet why_eq_l8_l3 = TracebackUtils::why_ancestor(l3, l8);
        REQUIRE((
            why_eq_l8_l3.size() == 5 &&
            why_eq_l8_l3.contains(base_pred) &&
            why_eq_l8_l3.contains(preds[0]) &&  // coll A B C
            why_eq_l8_l3.contains(preds[3]) &&  // coll I G D
            why_eq_l8_l3.contains(preds[4]) &&  // base I C
            why_eq_l8_l3.contains(preds[5])     // base D A
        ));

        PredSet why_on_h_l8 = tr.why_on(h, l8);
        REQUIRE((
            why_on_h_l8.size() == 6 &&
            why_on_h_l8.contains(base_pred) &&
            why_on_h_l8.contains(preds[0]) &&  // coll A B C
            why_on_h_l8.contains(preds[1]) &&  // coll H B D
            why_on_h_l8.contains(preds[3]) &&  // coll I G D
            why_on_h_l8.contains(preds[4]) &&  // base I C
            why_on_h_l8.contains(preds[5])     // base D A
        ));

        PredSet why_on_b_l8 = tr.why_on(b, l8);
         REQUIRE((
            why_on_b_l8.size() == 5 &&
            why_on_b_l8.contains(base_pred) &&
            why_on_b_l8.contains(preds[0]) &&  // coll A B C
            why_on_b_l8.contains(preds[3]) &&  // coll I G D
            why_on_b_l8.contains(preds[4]) &&  // base I C
            why_on_b_l8.contains(preds[5])     // base D A
        ));

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{i, e, d})
        ));
        /* coll D E I */
        ggraph.synthesise_preds(dd, ar);

        REQUIRE((
            NodeUtils::get_parent(l5) == l4 && NodeUtils::get_parent(l7) == l4 && NodeUtils::get_parent(l8) == l4 &&
            NodeUtils::get_root(l2) == l4 &&
            l4->children.size() == 3 && l5->children.size() == 0 && l7->children.size() == 0 && l8->children.size() == 3
        ));
        REQUIRE(l4->points.size() == 7);

        PredSet why_eq_l4_l8 = TracebackUtils::why_ancestor(l8, l4);
        REQUIRE((
            why_eq_l4_l8.size() == 3 &&
            why_eq_l4_l8.contains(base_pred) &&
            why_eq_l4_l8.contains(preds[3]) &&  // coll I G D
            why_eq_l4_l8.contains(preds[6])     // coll D E I
        ));

        /* E = G */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{e, g}
            )
        ));
        ggraph.merge_points(e, g, preds.back(), dd, ar);

        /* E = B */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{e, b}
            )
        ));
        ggraph.merge_points(e, b, preds.back(), dd, ar);

        /* C = F */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{c, f}
            )
        ));
        ggraph.merge_points(c, f, preds.back(), dd, ar);

        /* H = C */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{h, c}
            )
        ));
        ggraph.merge_points(h, c, preds.back(), dd, ar);

        REQUIRE((
            NodeUtils::get_parent(l5) == l4 && NodeUtils::get_parent(l7) == l4 && NodeUtils::get_parent(l8) == l4 &&
            NodeUtils::get_parent(l10) == l8 && NodeUtils::get_parent(l9) == l8 && NodeUtils::get_parent(l3) == l8 &&
            NodeUtils::get_parent(l6) == l3 && NodeUtils::get_parent(l1) == l3 &&
            NodeUtils::get_parent(l2) == l1 &&
            NodeUtils::get_root(l2) == l4 &&
            l4->children.size() == 3 && 
            l5->children.size() == 0 && l7->children.size() == 0 && l8->children.size() == 3 &&
            l10->children.size() == 0 && l9->children.size() == 0 && l3->children.size() == 2 &&
            l6->children.size() == 0 && l1->children.size() == 1 && l2->children.size() == 0
        ));

        REQUIRE((
            NodeUtils::get_parent(a) == d &&
            NodeUtils::get_parent(c) == i && NodeUtils::get_parent(f) == i && NodeUtils::get_parent(i) == h &&
            NodeUtils::get_parent(b) == e && NodeUtils::get_parent(g) == e
        ));

        /* preds = {
            0: coll C B A,
            1: coll H B D, 
            2: coll F E D, 
            3: coll I G D,
            4: base I C,
            5: base D A,
            6: coll D E I,
            7: base E G,
            8: base E B,
            9: base C F,
            10: base H C
        } */

        // preds[0]: coll A B C
        PredSet why_coll_abc = tr.why_coll(a, b, c);

        std::cout << why_coll_abc.to_string() << std::endl;
        
        REQUIRE((
            why_coll_abc.size() == 2 &&
            why_coll_abc.contains(base_pred) &&
            why_coll_abc.contains(preds[0])
        ));

        /* tr.why_coll(d, g, c) is an invalid call. This is because 
        a) coll D G I is first declared,
        b) then I = C.
        Before b) occurs, C is still a root point, but coll D G C is not true.
        After b) occurs, C is no longer a root point. */

        PredSet why_coll_dbi = tr.why_coll(d, b, i);
        REQUIRE((
            why_coll_dbi.size() == 4 &&
            why_coll_dbi.contains(base_pred) &&
            why_coll_dbi.contains(preds[0]) &&  // coll A B C
            why_coll_dbi.contains(preds[4]) &&  // base I C
            why_coll_dbi.contains(preds[5])     // base D A
        ));

        PredSet why_coll_bgh = tr.why_coll(b, g, h);
        REQUIRE((
            why_coll_bgh.size() == 6 &&
            why_coll_bgh.contains(base_pred) &&
            why_coll_bgh.contains(preds[0]) &&  // coll A B C
            why_coll_bgh.contains(preds[1]) &&  // coll H B D
            why_coll_bgh.contains(preds[3]) &&  // coll I G D
            why_coll_bgh.contains(preds[4]) &&  // base I C
            why_coll_bgh.contains(preds[5])     // base D A
        ));

        PredSet why_coll_egh = tr.why_coll(e, g, h);
        std::cout << why_coll_egh.to_string() << std::endl;
    }
}