
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

        /* I = C */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{i, c}
            )
        ));
        ggraph.merge_points(i, c, preds.back(), ar);

        REQUIRE((
            NodeUtils::get_parent(l9) == l8 &&
            NodeUtils::get_root(l9) == l8 &&
            NodeUtils::get_parent(c) == i &&
            NodeUtils::get_root(c) == i
        ));

        /* D = A */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{d, a}
            )
        ));
        ggraph.merge_points(d, a, preds.back(), ar);

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

        /* E = G */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{e, g}
            )
        ));
        ggraph.merge_points(e, g, preds.back(), ar);

        /* E = B */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{e, b}
            )
        ));
        ggraph.merge_points(e, b, preds.back(), ar);

        /* C = F */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{c, f}
            )
        ));
        ggraph.merge_points(c, f, preds.back(), ar);

        /* H = C */
        preds.emplace_back(dd.insert_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{h, c}
            )
        ));
        ggraph.merge_points(h, c, preds.back(), ar);

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
            coll C B A,
            coll H B D, 
            coll F E D, 
            coll I G D,
            base I C,
            base D A,
            coll D E I,
            base E G,
            base E B,
            base C F,
            base H C
        } */

        // PredSet why_abc = tr.why_coll(a, b, c);
        
        // REQUIRE((
        //     why_abc.contains(base_pred) &&
        //     why_abc.contains(preds[0])
        // ));

        // PredSet why_acg = tr.why_coll(a, g, c);

        // REQUIRE((
        //     why_acg.contains(base_pred) &&
        //     why_acg.contains(preds[4]) &&
        //     why_acg.contains(preds[5])
        // ));

        // PredSet why_dgh = tr.why_coll(d, g, h);
    }
}