
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_eqangle()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {0, 0});
        Point* b = ggraph.__add_new_point("b", {0, 4});
        Point* c = ggraph.__add_new_point("c", {4, 4});
        Point* d = ggraph.__add_new_point("d", {4, 0});
        Point* e = ggraph.__add_new_point("e", {2, 4});
        Point* f = ggraph.__add_new_point("f", {4, 2});
        Point* g = ggraph.__add_new_point("g", {3, 4});
        Point* h = ggraph.__add_new_point("h", {6, 4});
        Point* i = ggraph.__add_new_point("i", {0, 2});
        Point* j = ggraph.__add_new_point("j", {8, 4});

        std::vector<Predicate*> preds;

        // Build the square ABCD
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{a, b, b, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{b, c, c, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{c, d, d, a})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{a, b, c, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{b, c, d, a})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PERP, std::vector<Node*>{a, b, b, c})
        ));

        // Build the other points
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{e, b, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{f, c, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{c, b, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{g, e, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{h, c, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::MIDP, std::vector<Node*>{i, a, b})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{a, f, j})
        ));
        ggraph.synthesise_preds(dd, ar);

        /*
        0 - cong A B B C
        1 - cong B C C D
        2 - cong C D D A
        3 - para A B C D
        4 - para B C D A
        5 - perp A B B C
        6 - midp E B C
        7 - midp F C D
        8 - midp C B J
        9 - midp G E C
        10 - midp H C J
        11 - midp I A B
        12 - coll A F J
        */

        Angle* daf = ggraph.get_or_add_angle(d, a, f, dd);
        Angle* ifa = ggraph.get_or_add_angle(i, f, a, dd);
        Angle* fic = ggraph.get_or_add_angle(f, i, c, dd);
        Angle* bci = ggraph.get_or_add_angle(b, c, i, dd);
        Angle* cjf = ggraph.get_or_add_angle(c, j, f, dd);

        Angle* eab = ggraph.get_or_add_angle(e, a, b, dd);
        Angle* hdc = ggraph.get_or_add_angle(h, d, c, dd);

        Angle* cde = ggraph.get_or_add_angle(c, d, e, dd);
        Angle* fag = ggraph.get_or_add_angle(f, a, g, dd);
        Angle* cfg = ggraph.get_or_add_angle(c, f, g, dd);

        /* Round 1 */

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{b, c, f, i})
        ));
        ggraph.synthesise_preds(dd, ar);
        REQUIRE((
            Angle::is_equal(daf, ifa) &&
            Angle::is_equal(fic, bci)
        ));
        
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{a, f, c, i})
        ));
        ggraph.synthesise_preds(dd, ar);
        REQUIRE(
            Angle::is_equal(ifa, fic)
        );
    }
}