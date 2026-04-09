
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_perp()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        dd.add_theorem_template_from_text("A B C : para A B A C, diff B C => coll A B C");

        Point* a = ggraph.__add_new_point("a", {1, 3});
        Point* b = ggraph.__add_new_point("b", {1, 0});
        Point* c = ggraph.__add_new_point("c", {2, 3});
        Point* d = ggraph.__add_new_point("d", {2, 1});
        Point* e = ggraph.__add_new_point("e", {2, 0});
        Point* f = ggraph.__add_new_point("f", {3, 2});
        Point* g = ggraph.__add_new_point("g", {3, 1});
        Point* h = ggraph.__add_new_point("h", {3, 0});
        Point* i = ggraph.__add_new_point("i", {0, 2});
        Point* j = ggraph.__add_new_point("j", {4, 2});
        Point* k = ggraph.__add_new_point("k", {0, 1});
        Point* l = ggraph.__add_new_point("l", {4, 1});
        Point* m = ggraph.__add_new_point("m", {2, 1});
        Point* n = ggraph.__add_new_point("n", {5, 2});

        std::vector<Predicate*> preds;

        Line* ab = ggraph.get_or_add_line(a, b, dd);
        Line* cd = ggraph.get_or_add_line(c, d, dd);
        Line* ce = ggraph.get_or_add_line(c, e, dd);
        Line* fg = ggraph.get_or_add_line(f, g, dd);
        Line* gh = ggraph.get_or_add_line(g, h, dd);
        Line* fi = ggraph.get_or_add_line(f, i, dd);
        Line* fj = ggraph.get_or_add_line(f, j, dd);
        Line* dk = ggraph.get_or_add_line(d, k, dd);
        Line* gl = ggraph.get_or_add_line(g, l, dd);
        Line* gm = ggraph.get_or_add_line(g, m, dd);
        Line* in = ggraph.get_or_add_line(i, n, dd);
        Line* me = ggraph.get_or_add_line(m, e, dd);

        /* Round 1 */

        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::PERP, std::vector<Node*>{a, b, f, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::PARA, std::vector<Node*>{a, b, c, d})
        ));
        ggraph.synthesise_preds(dd, ar);

        REQUIRE((
            Line::is_para(ab, cd) &&
            Line::is_perp(cd, fi)
        ));

        /*
        0 - perp A B F I
        1 - para A B C D
        */

        PredSet why_perp_d_cd_d_fi = tr.why_directions_perp(cd->get_direction(), fi->get_direction());
        REQUIRE((
            why_perp_d_cd_d_fi.contains(preds[0]) &&   // perp A B F I
            why_perp_d_cd_d_fi.size() == 1
        ));
        PredSet why_dir_of_cd = tr.why_direction_of(cd->get_direction(), cd);
        REQUIRE((
            why_dir_of_cd.contains(preds[1]) &&   // para A B C D
            why_dir_of_cd.contains(base_pred) &&
            why_dir_of_cd.size() == 2
        ));

        /* Round 2 */

        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::PARA, std::vector<Node*>{a, b, c, e})
        ));
        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::PERP, std::vector<Node*>{c, d, f, j})
        ));
        ggraph.synthesise_preds(dd, ar);

        dd.search(ggraph);  // apply para C D C E => coll C D E and para F I F J => coll F I J
        
        // We "hack" the system to force coll C E D and coll F I J
        dd.recent_predicates.emplace_front(dd.predicates["coll f i j"].get());
        dd.recent_predicates.emplace_front(dd.predicates["coll c e d"].get());
        REQUIRE(ggraph.synthesise_preds(dd, ar) == 2);
        preds.emplace_back(dd.predicates["coll c e d"].get());
        preds.emplace_back(dd.predicates["coll f i j"].get());

        /*
        2 - para A B C E
        3 - perp C D F J
        4 - coll C E D
        5 - coll F I J
        */

        REQUIRE((
            cd->contains(e) &&
            e->is_on(cd) &&
            NodeUtils::get_root(ce) == cd &&
            fj->contains(i) &&
            i->is_on(fj) &&
            NodeUtils::get_root(fi) == fj
        ));
        Direction* d_fj = fj->get_direction();
        Direction* d_fi = static_cast<Direction*>(d_fj->children[0]);
        PredSet why_eq_d_fj_d_fi = TracebackUtils::why_ancestor(d_fi, d_fj);
        REQUIRE((
            why_eq_d_fj_d_fi.contains(preds[0]) &&   // perp A B F I
            why_eq_d_fj_d_fi.contains(preds[1]) &&   // para A B C D
            why_eq_d_fj_d_fi.contains(preds[3]) &&   // perp C D F J
            why_eq_d_fj_d_fi.contains(base_pred) &&   
            why_eq_d_fj_d_fi.size() == 4
        ));

        Direction* d_vert = ab->get_direction();    // d_ab
        Direction* d_horiz = fj->get_direction();   // d_fj
        REQUIRE((
            d_vert->to_string() == "d_l_a_b" &&
            d_horiz->to_string() == "d_l_f_j"
        ));

        PredSet why_dir_of_ce = tr.why_direction_of(d_vert, ce);
        REQUIRE((
            why_dir_of_ce.contains(preds[2]) &&   // para A B C E
            why_dir_of_ce.contains(base_pred) &&
            why_dir_of_ce.size() == 2
        ));

        PredSet why_dir_of_fi = tr.why_direction_of(d_horiz, fi);
        REQUIRE((
            why_dir_of_fi.contains(preds[0]) &&   // perp A B F I
            why_dir_of_fi.contains(preds[1]) &&   // para A B C D
            why_dir_of_fi.contains(preds[3]) &&   // perp C D F J
            why_dir_of_fi.contains(base_pred) &&   
            why_dir_of_fi.size() == 4
        ));

        /* Round 3 */

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{m, e, f, g})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PERP, std::vector<Node*>{f, g, g, l})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PERP, std::vector<Node*>{g, h, d, k})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PERP, std::vector<Node*>{g, h, i, n})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{d, k, g, m})
        ));
        ggraph.synthesise_preds(dd, ar);

        // for (const auto& [pair, _] : tr.perp_directions) {
        //     std::cout << pair.first->to_string() << ", " << pair.second->to_string() << " | ";
        // }
        // std::cout << std::endl;
        // for (const auto& [pair, set] : tr.perp_directions_root_map) {
        //     std::cout << pair.first->to_string() << ", " << pair.second->to_string() << " = ";
        //     for (const auto& pair_ : set) {
        //         std::cout << pair_.first->to_string() << ", " << pair_.second->to_string() << "; ";
        //     }
        //     std::cout << " | ";
        // }
        // std::cout << std::endl;

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::BASE, std::vector<Node*>{d, m})
        ));
        ggraph.merge_points(d, m, preds.back(), dd, ar);

        dd.search(ggraph);  // apply para D K D G => coll D K G

        // We "hack" the system to force coll D K G
        dd.recent_predicates.emplace_front(dd.predicates["coll d k g"].get());
        REQUIRE(ggraph.synthesise_preds(dd, ar) == 1);
        preds.emplace_back(dd.predicates["coll d k g"].get());

        REQUIRE((
            NodeUtils::same_as(dk, gm) &&
            NodeUtils::same_as(me, ce)
        ));

        d_vert = ab->get_direction();   // d_ab
        d_horiz = fi->get_direction();  // d_fj
        REQUIRE((
            d_vert->to_string() == "d_l_a_b" &&
            d_horiz->to_string() == "d_l_f_j"
        ));

        Direction* d_horiz_2 = in->get_direction();  // d_in
        REQUIRE((
            d_horiz_2->to_string() == "d_l_i_n"
        ));

        /*
        6 - para M E F G
        7 - perp F G G L
        8 - perp G H D K 
        9 - perp G H I N 
        10 - para D K G M
        11 - eq D M
        12 - coll D K G 
        */

        PredSet why_dir_of_fg = tr.why_direction_of(d_vert, fg);
        REQUIRE((
            why_dir_of_fg.contains(preds[11]) &&   // coll D M
            why_dir_of_fg.contains(preds[1]) &&   // para A B C D
            why_dir_of_fg.contains(preds[4]) &&   // coll C E D
            why_dir_of_fg.contains(preds[6]) &&   // para M E F G
            why_dir_of_fg.contains(base_pred) &&   
            why_dir_of_fg.size() == 5
        ));

        why_dir_of_fi = tr.why_direction_of(d_horiz, fi);
        REQUIRE((
            why_dir_of_fi.contains(preds[0]) &&   // perp A B F I
            why_dir_of_fi.contains(preds[1]) &&   // para A B C D
            why_dir_of_fi.contains(preds[3]) &&   // perp C D F J
            why_dir_of_fi.contains(base_pred) &&   
            why_dir_of_fi.size() == 4
        )); // no change from previous analysis

        PredSet why_dir_of_gm = tr.why_direction_of(d_horiz_2, gm);
        REQUIRE((
            why_dir_of_gm.contains(preds[8]) &&   // perp G H D K
            why_dir_of_gm.contains(preds[9]) &&   // perp G H I N
            why_dir_of_gm.contains(preds[10]) &&   // para D K G M
            why_dir_of_gm.contains(base_pred)
        ));

        /* Round 4 */
        
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{n, i, j})
        ));
        ggraph.synthesise_preds(dd, ar);

        REQUIRE((
            n->is_on(fi) &&
            Line::is_para(ab, gh) &&
            Line::is_perp(fg, gm) &&
            Line::is_perp(gl, gh)
        ));

        /*
        13 - coll N I J
        */

        /*
        0 - perp A B F I        10 - para D K G M
        1 - para A B C D        11 - eq D M
        2 - para A B C E        12 - coll D K G 
        3 - perp C D F J        13 - coll N I J
        4 - coll C E D
        5 - coll F I J
        6 - para M E F G
        7 - perp F G G L
        8 - perp G H D K 
        9 - perp G H I N 
        */

        /* After round 1 */

        PredSet why_perp_cd_fi = tr.why_perp(c, d, f, i);
        REQUIRE((
            why_perp_cd_fi.contains(preds[0]) &&   // perp A B F I
            why_perp_cd_fi.contains(preds[1]) &&   // para A B C D
            why_perp_cd_fi.contains(base_pred) &&   
            why_perp_cd_fi.size() == 3
        ));

        /* After round 2 */

        PredSet why_para_cd_ce = tr.why_para(c, d, c, e);
        REQUIRE((
            why_para_cd_ce.contains(preds[1]) &&   // para A B C D
            why_para_cd_ce.contains(preds[2]) &&   // para A B C E
            why_para_cd_ce.contains(base_pred) &&   
            why_para_cd_ce.size() == 3
        ));

        PredSet why_para_fi_fj = tr.why_para(f, i, f, j);
        REQUIRE((
            why_para_fi_fj.contains(preds[0]) &&   // perp A B F I
            why_para_fi_fj.contains(preds[1]) &&   // para A B C D
            why_para_fi_fj.contains(preds[3]) &&   // perp C D F J
            why_para_fi_fj.contains(base_pred) &&   
            why_para_fi_fj.size() == 4
        ));

        PredSet why_perp_de_ij = tr.why_perp(d, e, i, j);
        REQUIRE((
            why_perp_de_ij.contains(preds[3]) &&   // perp C D F J
            why_perp_de_ij.contains(preds[4]) &&   // coll C E D
            why_perp_de_ij.contains(preds[5]) &&   // coll F I J
            why_perp_de_ij.contains(preds[1]) &&   // para A B C D - technically not necessary, but explains why CD assigned d_ab
            why_perp_de_ij.contains(base_pred) &&   
            why_perp_de_ij.size() == 5
        ));

        /* After round 3 */

        PredSet why_perp_de_gl = tr.why_perp(d, e, g, l);
        REQUIRE((
            why_perp_de_gl.contains(preds[7]) &&   // perp F G G L
            why_perp_de_gl.contains(preds[6]) &&   // para M E F G
            why_perp_de_gl.contains(preds[11]) &&  // eq D M
            why_perp_de_gl.contains(base_pred) &&   
            why_perp_de_gl.size() == 4
        ));

        PredSet why_perp_ab_gl = tr.why_perp(a, b, g, l);
        REQUIRE((
            why_perp_ab_gl.contains(preds[7]) &&   // perp F G G L
            why_perp_ab_gl.contains(preds[6]) &&   // para M E F G
            why_perp_ab_gl.contains(preds[11]) &&  // eq D M
            why_perp_ab_gl.contains(preds[1]) &&   // para A B C D
            why_perp_ab_gl.contains(preds[4]) &&   // coll C E D
            why_perp_ab_gl.contains(base_pred) &&   
            why_perp_ab_gl.size() == 6
        ));

        PredSet why_perp_fg_ij = tr.why_perp(f, g, i, j);
        REQUIRE((
            why_perp_fg_ij.contains(preds[6]) &&   // para M E F G
            why_perp_fg_ij.contains(preds[11]) &&  // eq D M
            why_perp_fg_ij.contains(preds[3]) &&   // perp C D F J
            why_perp_fg_ij.contains(preds[4]) &&   // coll C E D
            why_perp_fg_ij.contains(preds[5]) &&   // coll F I J
            why_perp_fg_ij.contains(preds[1]) &&   // perp A B C D - technically not necessary, but explains why FG has direction d_ab
            why_perp_fg_ij.contains(base_pred) &&   
            why_perp_fg_ij.size() == 7
        ));

        /* After round 4 */

        PredSet why_perp_gh_ij = tr.why_perp(g, h, i, j);
        REQUIRE((
            why_perp_gh_ij.contains(preds[13]) &&  // coll N I J
            why_perp_gh_ij.contains(preds[9]) &&   // perp G H I N
            why_perp_gh_ij.contains(base_pred)
        ));

        PredSet why_perp_in_cd = tr.why_perp(i, n, c, d);
        REQUIRE((
            why_perp_in_cd.contains(preds[13]) &&  // coll N I J
            why_perp_in_cd.contains(preds[5]) &&   // coll F I J
            why_perp_in_cd.contains(preds[3]) &&    // para C D F J
            why_perp_in_cd.contains(base_pred)
        ));

        PredSet why_perp_dk_de = tr.why_perp(d, k, d, e);
        REQUIRE((
            why_perp_dk_de.contains(preds[4]) &&    // coll C E D
            why_perp_dk_de.contains(preds[3]) &&    // perp C D F J
            why_perp_dk_de.contains(preds[5]) &&    // coll F I J
            why_perp_dk_de.contains(preds[13]) &&   // coll N I J
            why_perp_dk_de.contains(preds[9]) &&    // perp G H I N
            why_perp_dk_de.contains(preds[8]) &&    // perp G H D K
            why_perp_dk_de.contains(base_pred)
        ));

        PredSet why_para_gl_gm = tr.why_para(g, l, g, m);
        REQUIRE((
            why_para_gl_gm.contains(preds[10]) &&  // para D K G M
            why_para_gl_gm.contains(preds[8]) &&   // perp G H D K 
            why_para_gl_gm.contains(preds[9]) &&   // perp G H I N 
            why_para_gl_gm.contains(preds[13]) &&  // coll N I J
            why_para_gl_gm.contains(preds[5]) &&   // coll F I J
            why_para_gl_gm.contains(preds[4]) &&   // coll C E D
            why_para_gl_gm.contains(preds[6]) &&   // para M E F G
            why_para_gl_gm.contains(preds[11]) &&  // eq D M
            why_para_gl_gm.contains(preds[7]) &&   // perp F G G L
            (why_para_gl_gm.contains(preds[3]) ||  // perp C D F J
            (why_para_gl_gm.contains(preds[0]) &&  // perp A B F I
            why_para_gl_gm.contains(preds[1]))) && // para A B C D
            why_para_gl_gm.contains(base_pred)
        ));
    }
}