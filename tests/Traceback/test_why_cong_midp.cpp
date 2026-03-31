
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_cong() and why_midp()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {0, 0});
        Point* b = ggraph.__add_new_point("b", {1, 0});
        Point* c = ggraph.__add_new_point("c", {2, 0});
        Point* d = ggraph.__add_new_point("d", {3, 0});
        Point* e = ggraph.__add_new_point("e", {4, 0});
        Point* f = ggraph.__add_new_point("f", {5, 0});
        Point* g = ggraph.__add_new_point("g", {6, 0});
        Point* h = ggraph.__add_new_point("h", {7, 0});
        Point* i = ggraph.__add_new_point("i", {8, 0});
        Point* j = ggraph.__add_new_point("j", {8, 0}); // same as i

        std::vector<Predicate*> preds;

        /* Group 1 */
        
        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::CONG, std::vector<Node*>{a, b, b, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::COLL, std::vector<Node*>{a, b, c})
        ));
        ggraph.synthesise_preds(dd, ar);

        /* 
        0 - cong A B B C
        1 - coll A B C
        */

        Segment* ab = ggraph.get_or_add_segment(a, b, dd);
        Segment* bc = ggraph.get_or_add_segment(b, c, dd);
        Length* len_ab = ab->get_length();
        REQUIRE(len_ab->to_string() == "len_s_a_b");

        PredSet why_bc_len_ab = tr.why_length_of(len_ab, bc);
        REQUIRE((
            why_bc_len_ab.contains(preds[0]) &&    // cong A B B C
            why_bc_len_ab.contains(base_pred) &&
            why_bc_len_ab.size() == 2
        ));

        /* Group 2 */

        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::MIDP, std::vector<Node*>{d, c, e})
        ));
        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::MIDP, std::vector<Node*>{e, d, f})
        ));
        ggraph.synthesise_preds(dd, ar);

        /*
        2 - midp D C E
        3 - midp E D F
        */

        Segment* cd = ggraph.get_or_add_segment(c, d, dd);
        Segment* de = ggraph.get_or_add_segment(d, e, dd);
        Segment* ef = ggraph.get_or_add_segment(e, f, dd);
        Length* len_cd = cd->get_length();
        REQUIRE(len_cd->to_string() == "len_s_c_d");

        PredSet why_de_len_cd = tr.why_length_of(len_cd, de);
        REQUIRE((
            why_de_len_cd.contains(preds[2]) &&    // midp D C 
            why_de_len_cd.contains(base_pred) &&
            why_de_len_cd.size() == 2
        ));

        PredSet why_ef_len_cd = tr.why_length_of(len_cd, ef);
        REQUIRE((
            why_ef_len_cd.contains(preds[2]) &&    // midp D C E
            why_ef_len_cd.contains(preds[3]) &&    // midp E D F
            why_ef_len_cd.contains(base_pred) &&
            why_ef_len_cd.size() == 3
        ));

        /* Round 3 */

        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::CONG, std::vector<Node*>{f, h, g, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::MIDP, std::vector<Node*>{h, g, j})
        ));
        ggraph.synthesise_preds(dd, ar);

        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::COLL, std::vector<Node*>{f, g, h})
        ));
        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::BASE, std::vector<Node*>{i, j})
        ));
        ggraph.merge_points(i, j, preds.back(), dd, ar);
        ggraph.synthesise_preds(dd, ar);

        Line* fghi = ggraph.get_or_add_line(f, i, dd);
        REQUIRE((
            ggraph.get_or_add_line(f, g, dd) == fghi &&
            ggraph.get_or_add_line(g, h, dd) == fghi &&
            ggraph.get_or_add_line(h, i, dd) == fghi
        ));
        Segment* fh = ggraph.get_or_add_segment(f, h, dd);
        Segment* gi = ggraph.get_or_add_segment(g, i, dd);
        Segment* fg = ggraph.get_or_add_segment(f, g, dd);
        Segment* gh = ggraph.get_or_add_segment(g, h, dd);
        Segment* hi = ggraph.get_or_add_segment(h, i, dd);
        REQUIRE((
            fh->get_line() == fghi &&
            gi->get_line() == fghi &&
            fg->get_line() == fghi &&
            gh->get_line() == fghi &&
            hi->get_line() == fghi
        ));

        ar.derive(ggraph, dd);
        dd.recent_predicates.emplace_front(dd.predicates["cong f g g h"].get());
        ggraph.synthesise_ar_preds(dd);
        // 8 predicates get synthesised, including some eqratios and constratios
        preds.emplace_back(dd.predicates["cong f g g h"].get());

        /*
        4 - cong F H G I
        5 - midp H G J
        6 - coll F G H
        7 - eq I J
        8 - cong F G G H (AR derived)
        */

        PredSet why_i_endptof_hi = tr.why_endpoint(i, hi);
        REQUIRE(
            why_i_endptof_hi.contains(preds[7])    // eq I J
        );

        Length* len_fg = fg->get_length();
        REQUIRE(len_fg->to_string() == "len_s_f_g");
        REQUIRE((
            Length::is_cong(len_fg, gh->get_length()) &&
            Length::is_cong(len_fg, hi->get_length())
        ));

        PredSet why_hi_len_fg = tr.why_length_of(len_fg, hi);
        REQUIRE((
            why_hi_len_fg.contains(preds[5]) &&    // midp H G J
            why_hi_len_fg.contains(preds[8]) &&    // cong F G G H
            why_hi_len_fg.contains(base_pred) &&
            why_hi_len_fg.size() == 3
        ));
        // eq I J is missing from this, because tr.why_on(I, HI) = eq I J is not included in why_length_of(), which
        // only looks at the raw Segment object HI (and not its constituent endpoints).

        /* Round 4 */

        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::MIDP, std::vector<Node*>{c, b, d})
        ));
        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::COLL, std::vector<Node*>{d, f, h})
        ));
        preds.emplace_back(dd.insert_new_predicate(
                std::make_unique<Predicate>(
                    pred_t::CONG, std::vector<Node*>{c, d, h, i})
        ));
        ggraph.synthesise_preds(dd, ar);

        
        Length* len_1 = ab->get_length();
        Length* len_2 = fh->get_length();
        Segment* ce = ggraph.get_or_add_segment(c, e, dd);
        REQUIRE((
            NodeUtils::same_as(cd->get_length(), len_1) &&
            NodeUtils::same_as(ef->get_length(), len_1) &&
            NodeUtils::same_as(fg->get_length(), len_1) &&
            NodeUtils::same_as(gh->get_length(), len_1) &&
            NodeUtils::same_as(gi->get_length(), len_2)
        ));
        REQUIRE(!NodeUtils::same_as(len_1, len_2));

        /*
        9 - midp C B D
        10 - coll D F H
        11 - cong C D H I
        */

        PredSet why_midp_b_a_c = tr.why_midp(b, a, c);
        REQUIRE((
            why_midp_b_a_c.contains(preds[0]) &&    // cong A B B C
            why_midp_b_a_c.contains(preds[1]) &&    // coll A B C
            why_midp_b_a_c.contains(base_pred) &&
            why_midp_b_a_c.size() == 3
        ));

        PredSet why_cong_c_d_e_f = tr.why_cong(c, d, e, f);
        REQUIRE((
            why_cong_c_d_e_f.contains(preds[2]) &&    // midp D C E
            why_cong_c_d_e_f.contains(preds[3]) &&    // midp E D F
            why_cong_c_d_e_f.contains(base_pred) &&
            why_cong_c_d_e_f.size() == 3
        ));

        PredSet why_coll_c_e_f = tr.why_coll(c, e, f);
        REQUIRE((
            why_coll_c_e_f.contains(preds[2]) &&    // midp D C E
            why_coll_c_e_f.contains(preds[3]) &&    // midp E D F
            why_coll_c_e_f.contains(base_pred) &&
            why_coll_c_e_f.size() == 3
        ));

        PredSet why_midp_g_f_h = tr.why_midp(g, f, h);
        REQUIRE((
            why_midp_g_f_h.contains(preds[5]) &&    // midp H G J
            why_midp_g_f_h.contains(preds[6]) &&    // coll F G H
            why_midp_g_f_h.contains(preds[7]) &&    // eq I J
            why_midp_g_f_h.contains(preds[8]) &&    // cong F G G H
            why_midp_g_f_h.contains(base_pred) &&
            why_midp_g_f_h.size() == 5
        ));

        PredSet why_cong_f_g_h_i = tr.why_cong(f, g, h, i);
        REQUIRE((
            why_cong_f_g_h_i.contains(preds[5]) &&    // midp H G J
            why_cong_f_g_h_i.contains(preds[7]) &&    // eq I J
            why_cong_f_g_h_i.contains(preds[8]) &&    // cong F G G H
            why_cong_f_g_h_i.contains(base_pred) &&
            why_cong_f_g_h_i.size() == 4
        ));

        PredSet why_cong_b_c_c_d = tr.why_cong(b, c, c, d);
        REQUIRE((
            why_cong_b_c_c_d.contains(preds[9]) &&    // midp C B D
            why_cong_b_c_c_d.contains(base_pred)
            // cong A B B C is also in here, as the length stored is len_AB
        ));
        
        PredSet why_cong_a_b_d_e = tr.why_cong(a, b, d, e);
        REQUIRE((
            why_cong_a_b_d_e.contains(preds[0]) &&    // cong A B B C
            why_cong_a_b_d_e.contains(preds[2]) &&    // midp D C E
            why_cong_a_b_d_e.contains(preds[9]) &&    // midp C B D
            why_cong_a_b_d_e.contains(base_pred)
        ));

        PredSet why_cong_e_f_f_g = tr.why_cong(e, f, f, g);
        std::cout << "why_cong_e_f_f_g: " << why_cong_e_f_f_g.to_string() << std::endl;
        REQUIRE((
            why_cong_e_f_f_g.contains(preds[11]) &&   // cong C D H I
            why_cong_e_f_f_g.contains(preds[2]) &&    // midp D C E
            why_cong_e_f_f_g.contains(preds[3]) &&    // midp E D F - explains CD = DE = EF
            why_cong_e_f_f_g.contains(preds[5]) &&    // midp H G J - explains HI = GH
            why_cong_e_f_f_g.contains(preds[8]) &&    // cong F G G H (contains eq I J as a prerequisite)
            why_cong_e_f_f_g.contains(base_pred)
        ));

        PredSet why_midp_f_e_g = tr.why_midp(f, e, g);
        std::cout << "why_midp_f_e_g: " << why_midp_f_e_g.to_string() << std::endl;
        REQUIRE((
            why_midp_f_e_g.contains(preds[11]) &&   // cong C D H I
            why_midp_f_e_g.contains(preds[2]) &&    // midp D C E
            why_midp_f_e_g.contains(preds[3]) &&    // midp E D F - explains CD = DE = EF
            why_midp_f_e_g.contains(preds[5]) &&    // midp H G J - explains HI = GH
            why_midp_f_e_g.contains(preds[8]) &&    // cong F G G H (contains eq I J as a prerequisite)
            why_midp_f_e_g.contains(preds[10]) &&   // coll D F H
            why_midp_f_e_g.contains(base_pred)
        ));
    }
}