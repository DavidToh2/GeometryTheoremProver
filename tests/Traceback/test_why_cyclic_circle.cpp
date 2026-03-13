
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_cyclic() and why_circle()") {
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

        Point* w = ggraph.__add_new_point("w", {0, 0});
        Point* x = ggraph.__add_new_point("x", {0, 0});
        Point* y = ggraph.__add_new_point("y", {0, 0});
        Point* z = ggraph.__add_new_point("z", {0, 0});

        Circle* abc = ggraph.__add_new_circle(a, b, c, base_pred);
        Circle* bcd = ggraph.__add_new_circle(b, c, d, base_pred);
        Circle* abh = ggraph.__add_new_circle(a, b, h, base_pred);
        Circle* bhd = ggraph.__add_new_circle(b, h, d, base_pred);
        Circle* fbi = ggraph.__add_new_circle(f, b, i, base_pred);
        Circle* fmn = ggraph.__add_new_circle(f, m, n, base_pred);
        Circle* gce = ggraph.__add_new_circle(g, c, e, base_pred);
        Circle* fgi = ggraph.__add_new_circle(f, g, i, base_pred);
        Circle* kln = ggraph.__add_new_circle(k, l, n, base_pred);
        Circle* lmo = ggraph.__add_new_circle(l, m, o, base_pred);

        std::vector<Predicate*> preds;

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{w, b, c, d})
        ));
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
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{x, f, b, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{f, m, n, e})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{y, g, c, e})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{z, f, g, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{k, l, n, o})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CIRCLE, std::vector<Node*>{x, k, l, n})
        ));

        /* 
        1 - cyclic A B C D,      0 - circle W B C D, 
        2 - cyclic A B H I,      5 - circle X F B I,
        3 - cyclic B H D J,      6 - circle Y G C E,
        4 - cyclic F B I E,      8 - circle Z F G I, 
        7 - cyclic F M N E,      10 - circle X K L N
        9 - cyclic K L N O */
        ggraph.synthesise_preds(dd, ar);

        REQUIRE(ggraph.root_circles.size() == 9);

        PredSet why_on_d_abc = tr.why_on(d, abc);
        REQUIRE((
            why_on_d_abc.size() == 2 &&
            why_on_d_abc.contains(base_pred) &&  // because d belongs to bcd which was merged into abc
            why_on_d_abc.contains(preds[1])      // cyclic A B C D
        ));

        PredSet why_on_i_abh = tr.why_on(i, abh);
        REQUIRE((
            why_on_i_abh.size() == 1 &&
            why_on_i_abh.contains(preds[2])      // cyclic A B H I
        ));

        PredSet why_cyclic_abhi = tr.why_cyclic(a, b, h, i);
        REQUIRE((
            why_cyclic_abhi.size() == 2 &&
            why_cyclic_abhi.contains(base_pred) &&
            why_cyclic_abhi.contains(preds[2])      // cyclic A B H I
        ));

        PredSet why_circle_z_fgi = tr.why_circle(z, f, g, i);
        REQUIRE((
            why_circle_z_fgi.size() == 2 &&
            why_circle_z_fgi.contains(base_pred) &&
            why_circle_z_fgi.contains(preds[8])      // circle Z F G I
        ));

        PredSet why_circle_w_abc = tr.why_circle(w, a, b, c);
        REQUIRE((
            why_circle_w_abc.size() == 3 &&
            why_circle_w_abc.contains(base_pred) &&
            why_circle_w_abc.contains(preds[0]) &&   // circle W B C D
            why_circle_w_abc.contains(preds[1])      // cyclic A B C D
        ));

        // 11 - cyclic B H I J
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{b, h, i, j})
        ));
        ggraph.synthesise_preds(dd, ar);

        Circle* abcdhij = NodeUtils::get_root(abc);
        REQUIRE((
            abcdhij == bhd &&
            NodeUtils::get_root(abh) == abcdhij &&
            NodeUtils::get_root(bhd) == abcdhij &&
            ggraph.get_or_add_circle(c, i, j, dd) == abcdhij &&
            abcdhij->get_center() == w
        ));

        PredSet why_cyclic_abhj = tr.why_cyclic(a, b, h, j);
        REQUIRE((
            why_cyclic_abhj.size() == 4 &&
            why_cyclic_abhj.contains(base_pred) &&
            why_cyclic_abhj.contains(preds[2]) &&     // cyclic A B H I
            why_cyclic_abhj.contains(preds[3]) &&     // cyclic B H D J
            why_cyclic_abhj.contains(preds[11])       // cyclic B H I J
        ));

        PredSet why_cyclic_adij = tr.why_cyclic(a, d, i, j);
        REQUIRE((
            why_cyclic_adij.size() == 4 &&
            why_cyclic_adij.contains(base_pred) &&
            why_cyclic_adij.contains(preds[2]) &&     // cyclic A B H I
            why_cyclic_adij.contains(preds[3]) &&     // cyclic B H D J
            why_cyclic_adij.contains(preds[11])       // cyclic B H I J
        ));

        PredSet why_cyclic_cdhi = tr.why_cyclic(c, d, h, i);
        /* C, D belong to circle ABCD; H, I to circle ABHI; these two circles have least common ancestor BHDJ
        ABHI -> BHDJ because 
            [ cyclic BHIJ + why_on(ABHI, {B, H, I}) + why_on(BHDJ, {B, H, J}) ]
        ABCD -> BHDJ because the circle merger algorithm identified that ABCD has A, B in common with ABHI and 
        B, D in common with BHDJ, so we have 
            [ cyclic BHIJ + why_on(ABCD, {A, B}) + why_on(ABHI, {A, B}) + why_on(ABCD, {B, D}) 
            + why_on(BHDJ, {B, D}) + diff A B + diff B D ]*/
        Predicate* diff_a_b = dd.predicates["diff a b"].get();
        Predicate* diff_b_d = dd.predicates["diff b d"].get();
        REQUIRE((
            why_cyclic_cdhi.size() == 6 &&
            why_cyclic_cdhi.contains(base_pred) &&
            why_cyclic_cdhi.contains(preds[1]) &&     // cyclic A B C D
            why_cyclic_cdhi.contains(preds[2]) &&     // cyclic A B H I
            why_cyclic_cdhi.contains(preds[11]) &&    // cyclic B H I J
            why_cyclic_cdhi.contains(diff_a_b) &&
            why_cyclic_cdhi.contains(diff_b_d)
        ));

        PredSet why_cyclic_bcij = tr.why_cyclic(b, c, i, j);
        /* Similar analysis as above applies. B belongs to all three circles; C only to ABCD; I only to ABHI;
        J only to BHDJ */
        REQUIRE((
            why_cyclic_bcij.size() == 7 &&
            why_cyclic_bcij.contains(base_pred) &&
            why_cyclic_bcij.contains(preds[1]) &&     // cyclic A B C D
            why_cyclic_bcij.contains(preds[2]) &&     // cyclic A B H I
            why_cyclic_bcij.contains(preds[3]) &&     // cyclic B H D J
            why_cyclic_bcij.contains(preds[11]) &&    // cyclic B H I J
            why_cyclic_bcij.contains(diff_a_b) &&
            why_cyclic_bcij.contains(diff_b_d)
        ));

        // 12 - cyclic L M N O
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::CYCLIC, std::vector<Node*>{l, m, n, o})
        ));
        ggraph.synthesise_preds(dd, ar);

        Circle* klmno = NodeUtils::get_root(kln);
        REQUIRE((
            klmno == kln &&
            NodeUtils::get_root(lmo) == klmno &&
            ggraph.get_or_add_circle(l, m, n, dd) == klmno &&
            klmno->get_center() == x
        ));

        PredSet why_cyclic_lmno = tr.why_cyclic(l, m, n, o);
        REQUIRE((
            why_cyclic_lmno.size() == 2 &&
            why_cyclic_lmno.contains(base_pred) &&
            why_cyclic_lmno.contains(preds[12])       // cyclic L M N O
        ));

        PredSet why_circle_x_klm = tr.why_circle(x, k, l, m);
        REQUIRE((
            why_circle_x_klm.size() == 3 &&
            why_circle_x_klm.contains(base_pred) &&
            why_circle_x_klm.contains(preds[10]) &&    // circle X K L N
            why_circle_x_klm.contains(preds[12])       // cyclic L M N O
        ));

        // 13 - eq I N
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{i, n})
        ));
        ggraph.merge_points(i, n, preds.back(), dd, ar);

        
    }
}