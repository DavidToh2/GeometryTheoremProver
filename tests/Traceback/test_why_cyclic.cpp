
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_cyclic()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {-1, 0});
        Point* b = ggraph.__add_new_point("b", {-1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* c = ggraph.__add_new_point("c", {0, 1});
        Point* d = ggraph.__add_new_point("d", {1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* e = ggraph.__add_new_point("e", {1, 0});
        Point* f = ggraph.__add_new_point("f", {-1, 0});
        Point* g = ggraph.__add_new_point("g", {-1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* h = ggraph.__add_new_point("h", {0, 1});
        Point* i = ggraph.__add_new_point("i", {1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* j = ggraph.__add_new_point("j", {1, 0});
        Point* k = ggraph.__add_new_point("k", {-1, 0});
        Point* l = ggraph.__add_new_point("l", {-1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* m = ggraph.__add_new_point("m", {0, 1});
        Point* n = ggraph.__add_new_point("n", {1.0 / std::sqrt(2), 1.0 / std::sqrt(2)});
        Point* o = ggraph.__add_new_point("o", {1, 0});

        Circle* abc = ggraph.__add_new_circle(a, b, c, base_pred);
        Circle* bcd = ggraph.__add_new_circle(b, c, d, base_pred);
        Circle* abh = ggraph.__add_new_circle(a, b, h, base_pred);
        Circle* bhd = ggraph.__add_new_circle(b, h, d, base_pred);
        Circle* fbi = ggraph.__add_new_circle(f, b, i, base_pred);
        Circle* gce = ggraph.__add_new_circle(g, c, e, base_pred);

        std::vector<Predicate*> preds;

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{d, c, b, a})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{a, b, h, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{b, h, d, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{f, b, i, e})
        ));

        /* cyclic A B C D, cyclic A B H I, cyclic B H D J, cyclic F B I E */
        ggraph.synthesise_preds(dd, ar);

        PredSet why_on_d_abc = tr.why_on(d, abc);
        REQUIRE((
            why_on_d_abc.size() == 2 &&
            why_on_d_abc.contains(base_pred) &&  // because d belongs to bcd which was merged into abc
            why_on_d_abc.contains(preds[0])      // cyclic A B C D
        ));

        PredSet why_on_i_abh = tr.why_on(i, abh);
        REQUIRE((
            why_on_i_abh.size() == 1 &&
            why_on_i_abh.contains(preds[1])      // cyclic A B H I
        ));

        PredSet why_cyclic_abhi = tr.why_cyclic(a, b, h, i);
        REQUIRE((
            why_cyclic_abhi.size() == 2 &&
            why_cyclic_abhi.contains(base_pred) &&
            why_cyclic_abhi.contains(preds[1])      // cyclic A B H I
        ));
    }
}