
#include <doctest.h>

#include "Geometry/GeometricGraph.hh"
#include "Geometry/Object2.hh"
#include "Traceback/TracebackEngine.hh"

TEST_SUITE("TracebackEngine: why_() functions") {
    TEST_CASE("why_eqratio()") {
        GeometricGraph ggraph;
        DDEngine dd;
        AREngine ar;
        TracebackEngine tr;
        ggraph.tr = &tr;
        Predicate* base_pred = dd.base_pred.get();

        Point* a = ggraph.__add_new_point("a", {0, 24});
        Point* b = ggraph.__add_new_point("b", {-18, 0});
        Point* c = ggraph.__add_new_point("c", {7, 0});
        Point* d = ggraph.__add_new_point("d", {-48.0/11, 0});
        Point* e = ggraph.__add_new_point("e", {-30, -16});
        Point* f = ggraph.__add_new_point("f", {35.0/3, -16});
        Point* g = ggraph.__add_new_point("g", {-108.0/11, -80.0/11});
        Point* h = ggraph.__add_new_point("h", {-20.0/11, -96.0/11});
        Point* i = ggraph.__add_new_point("i", {-180.0/11, -16});
        Point* j = ggraph.__add_new_point("j", {10.0/33, -16});
        Point* k = ggraph.__add_new_point("k", {-80.0/11, -16});
        Point* l = ggraph.__add_new_point("l", {-80.0/11, -16});

        std::vector<Predicate*> preds;

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{a, b, e})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{a, c, f})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{b, d, c})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{b, h, f})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{c, g, e})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{b, c, e, f})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{d, g, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{d, h, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{b, e, d, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{c, f, d, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{e, f, i})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{e, f, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQANGLE, std::vector<Node*>{b, a, a, d, d, a, a, c})
        ));
        ggraph.synthesise_preds(dd, ar);

        /*
        0 - coll A B E      10 - coll E F I
        1 - coll A C F      11 - coll E F J
        2 - coll B D C      12 - eqangle B A A D D A A C
        3 - coll B H F      
        4 - coll C G E
        5 - para B C E F
        6 - coll D G I
        7 - coll D H J
        8 - para B E D I
        9 - para C F D J
        */

        /* Round 1 */

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{b, d, d, c, b, a, a, c},
                PredSet{preds[12]})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{b, d, d, c, e, g, g, c},
                PredSet{preds[2], preds[4], preds[8]})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{b, d, d, c, b, h, h, f},
                PredSet{preds[2], preds[3], preds[9]})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{b, a, a, c, b, e, c, f},
                PredSet{preds[0], preds[1], preds[5]})
        ));
        ggraph.synthesise_preds(dd, ar);

        /*
        13 - eqratio B D D C B A A C
        14 - eqratio B D D C E G G C
        15 - eqratio B D D C B H H F
        16 - eqratio B A A C B E C F
        */

        Ratio* bd_dc = ggraph.get_or_add_ratio(b, d, d, c, dd);
        Fraction* frac = bd_dc->get_fraction();
        REQUIRE(frac->to_string() == "f_r_len_s_b_d_len_s_d_c");

        Ratio* ba_ac = ggraph.get_or_add_ratio(b, a, a, c, dd);
        Ratio* eg_gc = ggraph.get_or_add_ratio(e, g, g, c, dd);
        Ratio* bh_hf = ggraph.get_or_add_ratio(b, h, h, f, dd);
        Ratio* be_cf = ggraph.get_or_add_ratio(b, e, c, f, dd);

        PredSet why_ba_ac_frac_bd_dc = tr.why_fraction_of(frac, ba_ac);
        REQUIRE((
            why_ba_ac_frac_bd_dc.contains(preds[13]) &&    // eqratio B D D C B A A C
            why_ba_ac_frac_bd_dc.size() == 1
        ));

        PredSet why_be_cf_frac_bd_dc = tr.why_fraction_of(frac, be_cf);
        REQUIRE((
            why_be_cf_frac_bd_dc.contains(preds[16]) &&    // eqratio B A A C B E C F
            why_be_cf_frac_bd_dc.contains(preds[13]) &&    // eqratio B D D C B A A C
            why_be_cf_frac_bd_dc.size() == 2
        ));

        /* Round 2 */

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{e, g, g, c, i, g, g, d},
                PredSet{preds[4], preds[5], preds[6]})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{b, h, h, f, d, h, h, j},
                PredSet{preds[3], preds[5], preds[7]})
        ));
        ggraph.synthesise_preds(dd, ar);

        /* 
        17 - eqratio E G G C I G G D
        18 - eqratio B H H F D H H J
        */

        Ratio* ig_gd = ggraph.get_or_add_ratio(i, g, g, d, dd);
        Ratio* dh_hj = ggraph.get_or_add_ratio(d, h, h, j, dd);
        REQUIRE(Ratio::is_equal(ig_gd, dh_hj));

        PredSet why_ig_gd_frac_bd_dc = tr.why_fraction_of(frac, ig_gd);
        REQUIRE((
            why_ig_gd_frac_bd_dc.contains(preds[14]) &&    // eqratio B D D C E G G C
            why_ig_gd_frac_bd_dc.contains(preds[17]) &&    // eqratio E G G C I G G D
            why_ig_gd_frac_bd_dc.contains(base_pred) &&
            why_ig_gd_frac_bd_dc.size() == 3
        ));

        /* Round 3 */


        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{i, k, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::COLL, std::vector<Node*>{i, l, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{g, k, d, j})
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::PARA, std::vector<Node*>{h, l, d, i})
        ));
        ggraph.synthesise_preds(dd, ar);

        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{e, g, g, c, e, k, k, f},
                PredSet{preds[4], preds[19], preds[21]}
            )
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{b, h, h, f, e, l, l, f},
                PredSet{preds[3], preds[20], preds[22]}
            )
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{i, g, g, d, i, k, k, j},
                PredSet{preds[6], preds[19], preds[21]}
            )
        ));
        preds.emplace_back(dd.insert_new_predicate(
            std::make_unique<Predicate>(
                pred_t::EQRATIO, std::vector<Node*>{d, h, h, j, i, l, l, j},
                PredSet{preds[7], preds[20], preds[22]}
            )
        ));
        ggraph.synthesise_preds(dd, ar);

        Ratio* ik_kj = ggraph.get_or_add_ratio(i, k, k, j, dd);
        Ratio* il_lj = ggraph.get_or_add_ratio(i, l, l, j, dd);

        Ratio* ek_kf = ggraph.get_or_add_ratio(e, k, k, f, dd);
        Ratio* el_lf = ggraph.get_or_add_ratio(e, l, l, f, dd);
        REQUIRE((
            Ratio::is_equal(ik_kj, il_lj) &&
            Ratio::is_equal(ik_kj, ek_kf) &&
            Ratio::is_equal(ek_kf, el_lf)
        ));

        /*
        19 - coll I K J
        20 - coll I L J
        21 - para G K D J
        22 - para H L D I
        23 - eqratio E G G C E K K F
        24 - eqratio B H H F E L L F
        25 - eqratio I G G D I K K J
        26 - eqratio D H H J I L L J
        */

        PredSet why_ik_kj_frac_bd_dc = tr.why_fraction_of(frac, ik_kj);
        REQUIRE((
            why_ik_kj_frac_bd_dc.contains(preds[14]) &&    // eqratio B D D C E G G C
            why_ik_kj_frac_bd_dc.contains(preds[17]) &&    // eqratio E G G C I G G D
            why_ik_kj_frac_bd_dc.contains(preds[25]) &&    // eqratio I G G D I K K J
            why_ik_kj_frac_bd_dc.contains(base_pred) &&
            why_ik_kj_frac_bd_dc.size() == 4
        ));

        PredSet why_el_lf_frac_bd_dc = tr.why_fraction_of(frac, el_lf);
        REQUIRE((
            why_el_lf_frac_bd_dc.contains(preds[15]) &&    // eqratio B D D C B H H F
            why_el_lf_frac_bd_dc.contains(preds[24]) &&    // eqratio B H H F E L L F
            why_el_lf_frac_bd_dc.contains(base_pred) &&
            why_el_lf_frac_bd_dc.size() == 3
        ));

        /* 
        0 - coll A B E          10 - coll E F I                     20 - coll I L J
        1 - coll A C F          11 - coll E F J                     21 - para G K D J
        2 - coll B D C          12 - eqangle B A A D D A A C        22 - para H L D I
        3 - coll B H F          13 - eqratio B D D C B A A C        23 - eqratio E G G C E K K F
        4 - coll C G E          14 - eqratio B D D C E G G C        24 - eqratio B H H F E L L F
        5 - para B C E F        15 - eqratio B D D C B H H F        25 - eqratio I G G D I K K J
        6 - coll D G I          16 - eqratio B A A C B E C F        26 - eqratio D H H J I L L J
        7 - coll D H J          17 - eqratio E G G C I G G D
        8 - para B E D I        18 - eqratio B H H F D H H J
        9 - para C F D J        19 - coll I K J
        */

        /* After round 1 */

        PredSet why_eqratio_eg_gc_bh_hf = tr.why_eqratio(e, g, g, c, b, h, h, f);
        REQUIRE((
            why_eqratio_eg_gc_bh_hf.contains(preds[14]) &&    // eqratio B D D C E G G C
            why_eqratio_eg_gc_bh_hf.contains(preds[15]) &&    // eqratio B D D C B H H F
            why_eqratio_eg_gc_bh_hf.contains(base_pred) &&
            why_eqratio_eg_gc_bh_hf.size() == 3
        ));

        /* After round 2 */

        PredSet why_eqratio_ig_gd_dh_hj = tr.why_eqratio(i, g, g, d, d, h, h, j);
        REQUIRE((
            why_eqratio_ig_gd_dh_hj.contains(preds[14]) &&    // eqratio B D D C E G G C
            why_eqratio_ig_gd_dh_hj.contains(preds[17]) &&    // eqratio E G G C I G G D
            why_eqratio_ig_gd_dh_hj.contains(preds[15]) &&    // eqratio B D D C B H H F
            why_eqratio_ig_gd_dh_hj.contains(preds[18]) &&    // eqratio B H H F D H H J
            why_eqratio_ig_gd_dh_hj.contains(base_pred)
        ));

        PredSet why_eqratio_be_cf_ig_gd = tr.why_eqratio(b, e, c, f, i, g, g, d);
        REQUIRE((
            why_eqratio_be_cf_ig_gd.contains(preds[13]) &&    // eqratio B D D C B A A C
            why_eqratio_be_cf_ig_gd.contains(preds[16]) &&    // eqratio B A A C B E C F
            why_eqratio_be_cf_ig_gd.contains(preds[14]) &&    // eqratio B D D C E G G C
            why_eqratio_be_cf_ig_gd.contains(preds[17]) &&    // eqratio E G G C I G G D
            why_eqratio_be_cf_ig_gd.contains(base_pred)
        ));

        /* After round 3 */

        PredSet why_eqratio_ik_kj_ek_kf = tr.why_eqratio(i, k, k, j, e, k, k, f);
        REQUIRE((
            why_eqratio_ik_kj_ek_kf.contains(preds[23]) &&    // eqratio E G G C E K K F
            why_eqratio_ik_kj_ek_kf.contains(preds[17]) &&    // eqratio E G G C I G G D
            why_eqratio_ik_kj_ek_kf.contains(preds[25]) &&    // eqratio I G G D I K K J
            why_eqratio_ik_kj_ek_kf.contains(base_pred)
            // Extra eqratio B D D C E G G C present
        ));

        PredSet why_eqratio_ik_kj_il_lj = tr.why_eqratio(i, k, k, j, i, l, l, j);
        REQUIRE((
            why_eqratio_ik_kj_il_lj.contains(preds[14]) &&    // eqratio B D D C E G G C
            why_eqratio_ik_kj_il_lj.contains(preds[17]) &&    // eqratio E G G C I G G D
            why_eqratio_ik_kj_il_lj.contains(preds[25]) &&    // eqratio I G G D I K K J
            why_eqratio_ik_kj_il_lj.contains(preds[15]) &&    // eqratio B D D C B H H F
            why_eqratio_ik_kj_il_lj.contains(preds[18]) &&    // eqratio B H H F D H H J
            why_eqratio_ik_kj_il_lj.contains(preds[26]) &&    // eqratio D H H J I L L J
            why_eqratio_ik_kj_il_lj.contains(base_pred)
        ));
    }
}