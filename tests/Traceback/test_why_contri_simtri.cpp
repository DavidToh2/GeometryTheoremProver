
#include <doctest.h>
#include <memory>

#include "Common/Constants.hh"
#include "Geometry/GeometricGraph.hh"
#include "Geometry/Object2.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_contri() and why_simtri()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        Point* x = ggraph.__add_new_point("x", {0, 0});
        Point* a = ggraph.__add_new_point("a", {0, 1});
        Point* b = ggraph.__add_new_point("b", {1, 0});
        Point* c = ggraph.__add_new_point("c", {0, -1});
        Point* d = ggraph.__add_new_point("d", {-1, 0});
        Point* e = ggraph.__add_new_point("e", {0, 1});
        Point* f = ggraph.__add_new_point("f", {1, 0});
        Point* g = ggraph.__add_new_point("g", {0, -1});
        Point* h = ggraph.__add_new_point("h", {-1, 0});

        std::vector<Predicate*> preds;

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{x, a, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{x, b, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{x, e, g})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{x, f, h})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{x, a, x, b})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{x, b, x, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CONG, std::vector<Node*>{x, c, x, d})
        ));
        ggraph.synthesise_preds(dd, ar);

    }
}